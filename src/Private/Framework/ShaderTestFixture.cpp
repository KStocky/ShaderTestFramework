#include "Framework/ShaderTestFixture.h"

#include "Framework/TestDataBufferProcessor.h"
#include "Framework/HLSLTypes.h"
#include "Framework/PIXCapturer.h"
#include "Utility/EnumReflection.h"

#include "D3D12/CommandEngine.h"
#include "D3D12/GPUDevice.h"
#include "D3D12/Shader/ShaderReflectionUtils.h"

#include <d3d12shader.h>
#include <d3dx12/d3dx12.h>

#include <format>
#include <sstream>
#include <ranges>
#include <utility>

namespace stf
{
    namespace
    {
        ShaderCompiler CreateShaderCompiler(std::vector<VirtualShaderDirectoryMapping> InMappings)
        {
            fs::path shaderDir = std::filesystem::current_path();
            shaderDir += "/";
            shaderDir += SHADER_SRC;
            InMappings.push_back({ "/Test", std::move(shaderDir) });

            return ShaderCompiler{ std::move(InMappings) };
        }
    }

    StatSystem ShaderTestFixture::statSystem;
    std::vector<TimedStat> ShaderTestFixture::cachedStats;

    ShaderTestFixture::ShaderTestFixture(FixtureDesc InParams)
        : m_Device(MakeShared<GPUDevice>(InParams.GPUDeviceParams))
        , m_Compiler(CreateShaderCompiler(std::move(InParams.Mappings)))
        , m_ByteReaderMap()
        , m_Defines()
    {
        cachedStats.clear();
        PopulateDefaultByteReaders();
    }

    ShaderTestFixture::~ShaderTestFixture() noexcept
    {
        cachedStats = statSystem.FlushTimedStats();
    }

    Results ShaderTestFixture::RunTest(RuntimeTestDesc InTestDesc)
    {
        ScopedDuration fullTest(std::format("ShaderTestFixture::RunTest: {}", InTestDesc.TestName));

        InTestDesc.CompilationEnv.Defines.push_back(
            ShaderMacro
            {
                .Name = "TTL_STRING_MAX_LENGTH",
                .Definition = std::to_string(static_cast<i32>(InTestDesc.StringMaxLength))
            });

        const bool requestedRetryOnFail =
            InTestDesc.GPUCaptureMode == EGPUCaptureMode::CaptureOnFailure ||
            InTestDesc.StringMode == EStringMode::OnFailure;

        if (!requestedRetryOnFail)
        {
            return RunTestImpl(std::move(InTestDesc), false);
        }

        if (auto firstResult = RunTestImpl(InTestDesc, false))
        {
            return firstResult;
        }
        else if (auto testRunError = firstResult.GetTestRunError())
        {
            // There is no point in retrying the test if the test setup or shader compilation failed.
            return firstResult;
        }

        return RunTestImpl(std::move(InTestDesc), true);
    }

    Results ShaderTestFixture::RunCompileTimeTest(CompileTestDesc InTestDesc)
    {
        ScopedDuration scope(std::format("ShaderTestFixture::RunCompileTimeTest: {}", InTestDesc.TestName));
        const auto compileResult = CompileShader("", EShaderType::Lib, std::move(InTestDesc.CompilationEnv), false);

        if (!compileResult.has_value())
        {
            return Results{ { .Type = ETestRunErrorType::ShaderCompilation, .Error = compileResult.error() } };
        }

        return Results{ TestRunResults{} };
    }

    bool ShaderTestFixture::IsValid() const
    {
        return m_Device->IsValid();
    }

    bool ShaderTestFixture::IsUsingAgilitySDK() const
    {
        if (!IsValid())
        {
            return false;
        }

        return m_Device->GetHardwareInfo().FeatureInfo.EnhancedBarriersSupport;
    }

    std::vector<TimedStat> ShaderTestFixture::GetTestStats()
    {
        return cachedStats;
    }

    Results ShaderTestFixture::RunTestImpl(RuntimeTestDesc InTestDesc, const bool InIsFailureRetry)
    {
        const bool takeCapture = ShouldTakeCapture(InTestDesc.GPUCaptureMode, InIsFailureRetry);
        const bool enableStrings = InTestDesc.StringMode == EStringMode::On || (InIsFailureRetry && InTestDesc.StringMode == EStringMode::OnFailure);
        InTestDesc.CompilationEnv.Defines.push_back(
            ShaderMacro
            {
                .Name = "TTL_ENABLE_STRINGS",
                .Definition = enableStrings ? "1" : "0"
            }
        );
        const auto compileResult = CompileShader(InTestDesc.TestName, EShaderType::Compute, std::move(InTestDesc.CompilationEnv), takeCapture);

        if (!compileResult.has_value())
        {
            return Results
            {
                {
                    .Type = ETestRunErrorType::ShaderCompilation,
                    .Error = compileResult.error()
                }
            };
        }

        auto engine = CreateCommandEngine();
        auto resourceHeap = CreateDescriptorHeap();
        auto reflectionData = ProcessShaderReflection(compileResult.value());

        if (!reflectionData)
        {
            return Results{ reflectionData.error() };
        }
        auto pipelineState = CreatePipelineState(reflectionData.value().RootSig, compileResult->GetCompiledShader());

        const auto [dimX, dimY, dimZ] =
            [&compileResult, threadGroupCount = InTestDesc.ThreadGroupCount]()
            {
                u32 threadX = 0;
                u32 threadY = 0;
                u32 threadZ = 0;
                compileResult->GetReflection()->GetThreadGroupSize(&threadX, &threadY, &threadZ);

                const u32 dimX = threadX * threadGroupCount.x;
                const u32 dimY = threadY * threadGroupCount.y;
                const u32 dimZ = threadZ * threadGroupCount.z;

                return Tuple{ dimX, dimY, dimZ };
            }();

        const TestDataBufferLayout testDataLayout{ InTestDesc.TestDataLayout };

        if (reflectionData.value().RootParamBuffers.size() > 0)
        {
            InTestDesc.Bindings.append_range
            (std::array{
                ShaderBinding{ "stf::detail::DispatchDimensions", uint3{dimX, dimY, dimZ} },
                ShaderBinding{ "stf::detail::AllocationBufferIndex", 1u },
                ShaderBinding{ "stf::detail::TestDataBufferIndex", 0u },
                ShaderBinding{ "stf::detail::Asserts", testDataLayout.GetAssertSection() },
                ShaderBinding{ "stf::detail::Strings", testDataLayout.GetStringSection() },
                ShaderBinding{ "stf::detail::Sections", testDataLayout.GetSectionInfoSection() }
            });

            const auto populateResult = PopulateTestConstantBuffers(reflectionData.value(), InTestDesc.Bindings);
            if (!populateResult)
            {
                return Results{ populateResult.error() };
            }
        }

        const u64 bufferSizeInBytes = std::max(testDataLayout.GetSizeOfTestData(), 4u);
        auto assertBuffer = CreateAssertBuffer(bufferSizeInBytes);
        auto allocationBuffer = CreateAssertBuffer(28ull);
        auto readBackBuffer = CreateReadbackBuffer(bufferSizeInBytes);
        auto readBackAllocationBuffer = CreateReadbackBuffer(28ull);

        const auto assertUAV = CreateAssertBufferUAV(*assertBuffer, resourceHeap, 0);
        const auto allocationUAV = CreateAssertBufferUAV(*allocationBuffer, resourceHeap, 1);

        {
            ScopedDuration testExecution("ShaderTestFixture::RunTest Test Execution");
            const auto capturer = PIXCapturer(InTestDesc.TestName, takeCapture);

            engine.Execute(InTestDesc.TestName,
                [&resourceHeap,
                &pipelineState,
                &reflectionData,
                &assertBuffer,
                &allocationBuffer,
                &readBackBuffer,
                &readBackAllocationBuffer,
                threadGroupCount = InTestDesc.ThreadGroupCount,
                this]
                (ScopedCommandContext& InContext)
                {
                    InContext.Section("Test Setup",
                        [&](ScopedCommandContext& InContext)
                        {
                            InContext->SetPipelineState(pipelineState);
                            InContext->SetDescriptorHeaps(resourceHeap);
                            InContext->SetComputeRootSignature(reflectionData.value().RootSig);
                            InContext->SetBufferUAV(*assertBuffer);
                            InContext->SetBufferUAV(*allocationBuffer);

                            for (const auto& [paramIndex, buffer] : reflectionData.value().RootParamBuffers)
                            {
                                InContext->SetComputeRoot32BitConstants(paramIndex, std::span{ buffer }, 0);
                            }
                        }
                    );

                    InContext.Section("Test Dispatch",
                        [threadGroupCount](ScopedCommandContext& InContext)
                        {
                            InContext->Dispatch(threadGroupCount.x, threadGroupCount.y, threadGroupCount.z);
                        }
                    );

                    InContext.Section("Results readback",
                        [&](ScopedCommandContext& InContext)
                        {
                            InContext->CopyBufferResource(*readBackBuffer, *assertBuffer);
                            InContext->CopyBufferResource(*readBackAllocationBuffer, *allocationBuffer);
                        }
                    );
                }
            );

            engine.Flush();
        }

        {
            ScopedDuration readbackScope(std::format("ShaderTestFixture::ReadbackResults: {}", InTestDesc.TestName));
            return ReadbackResults(*readBackAllocationBuffer, *readBackBuffer, uint3(dimX, dimY, dimZ), testDataLayout);
        }
    }

    CompilationResult ShaderTestFixture::CompileShader(const std::string_view InName, const EShaderType InType, CompilationEnvDesc InCompileDesc, const bool InTakingCapture) const
    {
        ScopedDuration scope(std::format("ShaderTestFixture::CompileShader: {}", InName));
        ShaderCompilationJobDesc job;
        job.AdditionalFlags = std::move(InCompileDesc.CompilationFlags);
        job.AdditionalFlags.emplace_back(L"-enable-16bit-types");
        job.AdditionalFlags.emplace_back(L"-Wno-c++14-extensions");
        job.AdditionalFlags.emplace_back(L"-Wno-c++1z-extensions");
        job.EntryPoint = InName;
        job.ShaderModel = InCompileDesc.ShaderModel;
        job.ShaderType = InType;
        job.Source = std::move(InCompileDesc.Source);
        job.HLSLVersion = InCompileDesc.HLSLVersion;
        job.Defines = m_Defines;
        job.Defines.append_range(InCompileDesc.Defines);

        if (InTakingCapture)
        {
            job.AdditionalFlags.emplace_back(L"-Qembed_debug");
            job.AdditionalFlags.emplace_back(L"-Zss");
            job.AdditionalFlags.emplace_back(L"-Zi");
            job.Flags = Enum::MakeFlags(EShaderCompileFlags::SkipOptimization, EShaderCompileFlags::O0);
        }

        return m_Compiler.CompileShader(job);
    }

    CommandEngine ShaderTestFixture::CreateCommandEngine() const
    {
        auto commandList = m_Device->CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
        D3D12_COMMAND_QUEUE_DESC queueDesc;
        queueDesc.NodeMask = 0;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        auto commandQueue = m_Device->CreateCommandQueue(queueDesc);

        return CommandEngine(CommandEngine::CreationParams{ std::move(commandList), std::move(commandQueue), m_Device });
    }

    void ShaderTestFixture::RegisterByteReader(std::string InTypeIDName, MultiTypeByteReader InByteReader)
    {
        const u32 typeId = static_cast<u32>(m_ByteReaderMap.size());
        m_Defines.push_back(ShaderMacro{ std::move(InTypeIDName), std::format("{}", typeId) });
        m_ByteReaderMap.push_back(std::move(InByteReader));
    }

    void ShaderTestFixture::RegisterByteReader(std::string InTypeIDName, SingleTypeByteReader InByteReader)
    {
        RegisterByteReader(std::move(InTypeIDName),
            [byteReader = std::move(InByteReader)](const u16, const std::span<const std::byte> InData)
            {
                return byteReader(InData);
            }
        );
    }

    DescriptorHeap ShaderTestFixture::CreateDescriptorHeap() const
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.NodeMask = 0;
        desc.NumDescriptors = 2;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        return m_Device->CreateDescriptorHeap(desc);
    }

    PipelineState ShaderTestFixture::CreatePipelineState(const RootSignature& InRootSig, IDxcBlob* InShader) const
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC desc;
        desc.CachedPSO.CachedBlobSizeInBytes = 0;
        desc.CachedPSO.pCachedBlob = nullptr;
        desc.CS.BytecodeLength = InShader->GetBufferSize();
        desc.CS.pShaderBytecode = InShader->GetBufferPointer();
        desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
        desc.NodeMask = 0;
        desc.pRootSignature = InRootSig;
        return m_Device->CreatePipelineState(desc);
    }

    Expected<ShaderTestFixture::ReflectionResults, ErrorTypeAndDescription> ShaderTestFixture::ProcessShaderReflection(const CompiledShaderData& InShaderData) const
    {
        const auto refl = InShaderData.GetReflection();
        std::unordered_map<std::string, BindingInfo, TransparentStringHash, std::equal_to<>> nameToBindingInfo;
        std::unordered_map<u32, std::vector<u32>> rootParamBuffers;

        if (!refl)
        {
            return std::unexpected(ErrorTypeAndDescription
                {
                    .Type = ETestRunErrorType::RootSignatureGeneration,
                    .Error = "No reflection data generated. Did you compile your shader as a lib? Libs do not generate reflection data"
                });
        }

        D3D12_SHADER_DESC shaderDesc{};
        refl->GetDesc(&shaderDesc);

        std::vector<CD3DX12_ROOT_PARAMETER1> parameters;
        parameters.reserve(shaderDesc.BoundResources);

        u32 totalNumValues = 0;
        for (u32 boundIndex = 0; boundIndex < shaderDesc.BoundResources; ++boundIndex)
        {
            D3D12_SHADER_INPUT_BIND_DESC bindDesc{};
            refl->GetResourceBindingDesc(boundIndex, &bindDesc);

            if (bindDesc.Type != D3D_SIT_CBUFFER)
            {
                return std::unexpected(ErrorTypeAndDescription
                    {
                        .Type = ETestRunErrorType::RootSignatureGeneration,
                        .Error = "Only constant buffers are supported for binding."
                    });
            }

            const auto constantBuffer = refl->GetConstantBufferByName(bindDesc.Name);
            D3D12_SHADER_BUFFER_DESC bufferDesc{};
            ThrowIfFailed(constantBuffer->GetDesc(&bufferDesc));

            if (!ConstantBufferCanBeBoundToRootConstants(*constantBuffer))
            {
                return std::unexpected(ErrorTypeAndDescription
                    {
                        .Type = ETestRunErrorType::RootSignatureGeneration,
                        .Error = std::format("Constant Buffer: {} can not be stored in root constants", bufferDesc.Name)
                    });
            }

            const u32 numValues = bufferDesc.Size / sizeof(u32);
            totalNumValues += numValues;

            if (totalNumValues > 64)
            {
                return std::unexpected(ErrorTypeAndDescription
                    {
                        .Type = ETestRunErrorType::RootSignatureGeneration,
                        .Error = "Limit of 64 uints reached"
                    });
            }

            if (bufferDesc.Name != nullptr && std::string_view{ bufferDesc.Name } == std::string_view{ "$Globals" })
            {
                for (u32 globalIndex = 0; globalIndex < bufferDesc.Variables; ++globalIndex)
                {
                    auto var = constantBuffer->GetVariableByIndex(globalIndex);
                    D3D12_SHADER_VARIABLE_DESC varDesc{};
                    var->GetDesc(&varDesc);

                    nameToBindingInfo.emplace(
                        std::string{ varDesc.Name }, 
                        BindingInfo{ 
                            .RootParamIndex = static_cast<u32>(parameters.size()), 
                            .OffsetIntoBuffer = varDesc.StartOffset,
                            .BindingSize = varDesc.Size
                        });
                }
            }
            else
            {
                nameToBindingInfo.emplace(
                    std::string{ bindDesc.Name }, 
                    BindingInfo{ 
                        .RootParamIndex = static_cast<u32>(parameters.size()), 
                        .OffsetIntoBuffer = 0,
                        .BindingSize = bufferDesc.Size
                    });
            }

            rootParamBuffers[static_cast<u32>(parameters.size())].resize(bufferDesc.Size / sizeof(u32));

            auto& parameter = parameters.emplace_back();
            parameter.InitAsConstants(numValues, bindDesc.BindPoint, bindDesc.Space);
        }

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSig{};
        rootSig.Init_1_1(static_cast<u32>(parameters.size()), parameters.data(), 0u, nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED
        );

        return ReflectionResults{ 
            .RootSig = m_Device->CreateRootSignature(rootSig), 
            .NameToBindingInfo = std::move(nameToBindingInfo),
            .RootParamBuffers = std::move(rootParamBuffers)
        };
    }

    Expected<void, ErrorTypeAndDescription> ShaderTestFixture::PopulateTestConstantBuffers(
        ReflectionResults& InOutReflectionResults, 
        const std::span<const ShaderBinding> InBindings) const
    {
        for (const auto& binding : InBindings)
        {
            const auto bindingInfo = InOutReflectionResults.NameToBindingInfo.find(binding.GetName());
            if (bindingInfo == InOutReflectionResults.NameToBindingInfo.cend())
            {
                return Unexpected(ErrorTypeAndDescription
                {
                    .Type = ETestRunErrorType::Binding,
                    .Error = std::format("No shader binding named {} found in test shader", binding.GetName())
                });
            }

            auto& bindingBuffer = InOutReflectionResults.RootParamBuffers[bindingInfo->second.RootParamIndex];

            ThrowIfFalse(bindingBuffer.size() > 0, "Shader binding buffer has a size of zero. It should have been created and initialized when processing the reflection data");
        
            const auto bindingData = binding.GetBindingData();
            if (bindingData.size_bytes() > bindingInfo->second.BindingSize)
            {
                return
                    Unexpected(ErrorTypeAndDescription
                    {
                        .Type = ETestRunErrorType::Binding,
                        .Error = std::format("Shader binding {0} provided is larger than the expected binding size", binding.GetName())
                    });
            }

            const u32 uintIndex = bindingInfo->second.OffsetIntoBuffer / sizeof(u32);
            std::memcpy(bindingBuffer.data() + uintIndex, bindingData.data(), bindingData.size_bytes());
        }

        return {};
    }

    SharedPtr<GPUResource> ShaderTestFixture::CreateAssertBuffer(const u64 InSizeInBytes) const
    {
        const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        const auto resourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(InSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

        return m_Device->CreateCommittedResource(heapProps, D3D12_HEAP_FLAG_NONE, resourceDesc, D3D12_BARRIER_LAYOUT_UNDEFINED);
    }

    SharedPtr<GPUResource> ShaderTestFixture::CreateReadbackBuffer(const u64 InSizeInBytes) const
    {
        const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
        const auto resourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(InSizeInBytes);
        return m_Device->CreateCommittedResource(heapProps, D3D12_HEAP_FLAG_NONE, resourceDesc, D3D12_BARRIER_LAYOUT_UNDEFINED);
    }

    DescriptorHandle ShaderTestFixture::CreateAssertBufferUAV(const GPUResource& InAssertBuffer, const DescriptorHeap& InHeap, const u32 InIndex) const
    {
        const auto uav = ThrowIfUnexpected(InHeap.CreateDescriptorHandle(InIndex));
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        uavDesc.Buffer.CounterOffsetInBytes = 0;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
        uavDesc.Buffer.NumElements = static_cast<u32>(InAssertBuffer.GetDesc().Width) / 4;
        uavDesc.Buffer.StructureByteStride = 0;
        uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        m_Device->CreateUnorderedAccessView(InAssertBuffer, uavDesc, uav);
        return uav;
    }

    Results ShaderTestFixture::ReadbackResults(const GPUResource& InAllocationBuffer, const GPUResource& InAssertBuffer, const uint3 InDispatchDimensions, const TestDataBufferLayout& InTestDataLayout) const
    {
        const auto mappedAllocationData = InAllocationBuffer.Map();
        const auto allocationData = mappedAllocationData.Get();

        AllocationBufferData data;

        std::memcpy(&data, allocationData.data(), sizeof(AllocationBufferData));

        const auto mappedAssertData = InAssertBuffer.Map();
        const auto assertData = mappedAssertData.Get();

        return ProcessTestDataBuffer(data, InDispatchDimensions, InTestDataLayout, assertData, m_ByteReaderMap);
    }

    namespace ShaderTestFixturePrivate
    {
        template<typename T>
        struct GenericWriter
        {
            static void Write(std::stringstream& InOut, const std::byte*& InBytes)
            {
                T val;
                std::memcpy(&val, InBytes, sizeof(T));
                InOut << val;
                InBytes += sizeof(T);
            }
        };

        template<>
        struct GenericWriter<bool>
        {
            static void Write(std::stringstream& InOut, const std::byte*& InBytes)
            {
                u32 val;
                std::memcpy(&val, InBytes, sizeof(u32));
                InOut << (val != 0 ? "true" : "false");
                InBytes += sizeof(u32);
            }
        };
    }

    namespace
    {
        static const int NumSections = 32;

        enum class ESectionRunState
        {
            NeverEntered,
            NeedsRun,
            Running,
            RunningEnteredSubsection,
            RunningNeedsRerun,
            Completed
        };

        struct ScenarioSectionInfo
        {
            int ParentID;
            ESectionRunState RunState;
        };

        enum class EThreadIDType
        {
            None,
            Int,
            Int3
        };

        struct ThreadIDInfo
        {
            u32 Data;
            EThreadIDType Type;
        };

        struct PerThreadScratchData
        {
            i32 CurrentSectionID;
            i32 NextSectionID;
            i32 NextStringID;
            ThreadIDInfo ThreadID;
            ScenarioSectionInfo Sections[NumSections];
        };
    }

    void ShaderTestFixture::PopulateDefaultByteReaders()
    {
        RegisterByteReader("TYPE_ID_UNDEFINED",
            [](const u16, const std::span<const std::byte> InBytes)
            {
                return std::format("Undefined Type -> {}", DefaultByteReader(0, InBytes));
            });

        RegisterByteReader("READER_ID_PER_THREAD_SCRATCH",
            [](const std::span<const std::byte> InBytes)
            {
                PerThreadScratchData data;
                std::memcpy(&data, InBytes.data(), sizeof(PerThreadScratchData));

                std::stringstream buff;
                buff << "\nCurrentSectionID: " << data.CurrentSectionID << "\n";
                buff << "NextSectionID: " << data.NextSectionID << "\n";
                buff << "NextStringID: " << data.NextStringID << "\n";
                buff << "Sections:\n--------------------------------------\n";
                for (i32 i = 0; i < NumSections; ++i)
                {
                    buff << "Section " << i << "\n";
                    buff << "ParentID: " << data.Sections[i].ParentID << "\n";
                    const auto runState = Enum::UnscopedName(data.Sections[i].RunState);
                    buff << "RunState: " << runState << "\n-------------------------\n";
                }

                return buff.str();
            });

        RegisterByteReader("READER_ID_FUNDAMENTAL",
            [](const u16 InTypeId, const std::span<const std::byte> InBytes)
            {
                enum class EHLSLFundamentalBaseType
                {
                    Bool = 0,
                    Int,
                    Uint,
                    Float
                };

                enum class EHLSLFundamentalTypeBits
                {
                    Bit16,
                    Bit32,
                    Bit64
                };
                const auto type = static_cast<EHLSLFundamentalBaseType>(InTypeId & 0x3);
                const auto numBytes = static_cast<EHLSLFundamentalTypeBits>((InTypeId >> 2) & 3);
                const u32 numColumns = ((InTypeId >> 4) & 3) + 1;
                const u32 numRows = ((InTypeId >> 6) & 3) + 1;

                const auto generateMultiLengthConcreteWriter =
                    []<typename Length16, typename Length32, typename Length64>(const EHLSLFundamentalTypeBits InNumBits)
                {
                    switch (InNumBits)
                    {
                    case EHLSLFundamentalTypeBits::Bit16:
                    {
                        return ShaderTestFixturePrivate::GenericWriter<Length16>::Write;
                    }
                    case EHLSLFundamentalTypeBits::Bit32:
                    {
                        return ShaderTestFixturePrivate::GenericWriter<Length32>::Write;
                    }
                    case EHLSLFundamentalTypeBits::Bit64:
                    {
                        return ShaderTestFixturePrivate::GenericWriter<Length64>::Write;
                    }
                    default:
                        std::unreachable();
                    }
                };


                const auto concreteWriter =
                    [generateMultiLengthConcreteWriter](const EHLSLFundamentalBaseType InType, const EHLSLFundamentalTypeBits InNumBits)
                    {
                        switch (InType)
                        {
                        case EHLSLFundamentalBaseType::Bool:
                        {
                            return ShaderTestFixturePrivate::GenericWriter<bool>::Write;
                        }
                        case EHLSLFundamentalBaseType::Float:
                        {
                            return generateMultiLengthConcreteWriter.operator() < f16, f32, f64 > (InNumBits);
                        }
                        case EHLSLFundamentalBaseType::Int:
                        {
                            return generateMultiLengthConcreteWriter.operator() < i16, i32, i64 > (InNumBits);
                        }
                        case EHLSLFundamentalBaseType::Uint:
                        {
                            return generateMultiLengthConcreteWriter.operator() < u16, u32, u64 > (InNumBits);
                        }
                        default:
                            std::unreachable();
                        }
                    }(type, numBytes);


                std::stringstream ret;
                auto bytePointer = InBytes.data();
                if (numRows > 1)
                {
                    ret << "\n";
                }
                for ([[maybe_unused]] const auto row : std::views::iota(0u, numRows))
                {
                    concreteWriter(ret, bytePointer);
                    for ([[maybe_unused]] const auto column : std::views::iota(1u, numColumns))
                    {
                        ret << ", ";
                        concreteWriter(ret, bytePointer);
                    }
                    if (row != numRows - 1)
                    {
                        ret << "\n";
                    }
                }
                return ret.str();
            });
    }

    bool ShaderTestFixture::ShouldTakeCapture(const EGPUCaptureMode InCaptureMode, const bool InIsFailureRetry) const
    {
        const bool takeCaptureIfAble = InCaptureMode == EGPUCaptureMode::On || (InIsFailureRetry && InCaptureMode == EGPUCaptureMode::CaptureOnFailure);
        return m_Device->IsGPUCaptureEnabled() && takeCaptureIfAble;
    }
}

