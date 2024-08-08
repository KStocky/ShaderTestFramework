#include "Framework/ShaderTestFixture.h"

#include "Framework/TestDataBufferProcessor.h"
#include "Framework/HLSLTypes.h"
#include "Framework/PIXCapturer.h"
#include "Utility/EnumReflection.h"
#include "Utility/Math.h"

#include "D3D12/CommandEngine.h"
#include "D3D12/GPUDevice.h"

#include <format>
#include <sstream>
#include <ranges>
#include <utility>

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
    : m_Device(InParams.GPUDeviceParams)
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

STF::Results ShaderTestFixture::RunTest(RuntimeTestDesc InTestDesc)
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

    const auto firstResult = RunTestImpl(InTestDesc, false);
    const bool compilationFailed = firstResult.GetFailedCompilationResult();
    const bool succeeded = firstResult;

    if (succeeded || compilationFailed || !requestedRetryOnFail)
    {
        return firstResult;
    }
    
    return RunTestImpl(std::move(InTestDesc), true);
}

STF::Results ShaderTestFixture::RunCompileTimeTest(CompileTestDesc InTestDesc)
{
    ScopedDuration scope(std::format("ShaderTestFixture::RunCompileTimeTest: {}", InTestDesc.TestName));
    const auto compileResult = CompileShader("", EShaderType::Lib, std::move(InTestDesc.CompilationEnv), false);

    if (!compileResult.has_value())
    {
        return STF::Results{ {compileResult.error()} };
    }

    return STF::Results{ STF::TestRunResults{} };
}

bool ShaderTestFixture::IsValid() const
{
    return m_Device.IsValid();
}

bool ShaderTestFixture::IsUsingAgilitySDK() const
{
	if (!IsValid())
	{
		return false;
	}

	return m_Device.GetHardwareInfo().FeatureInfo.EnhancedBarriersSupport;
}

std::vector<TimedStat> ShaderTestFixture::GetTestStats()
{
    return cachedStats;
}

STF::Results ShaderTestFixture::RunTestImpl(RuntimeTestDesc InTestDesc, const bool InIsFailureRetry)
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
        return STF::Results
        {
            STF::FailedShaderCompilationResult{compileResult.error()}
        };
    }

    auto engine = CreateCommandEngine();
    auto resourceHeap = CreateDescriptorHeap();
    auto rootSignature = CreateRootSignature(compileResult.value());
    auto pipelineState = CreatePipelineState(rootSignature, compileResult->GetCompiledShader());

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

    const STF::TestDataBufferLayout testDataLayout{ InTestDesc.TestDataLayout };

    const u64 bufferSizeInBytes = std::max(testDataLayout.GetSizeOfTestData(), 4u);
    auto assertBuffer = CreateAssertBuffer(bufferSizeInBytes);
    auto allocationBuffer = CreateAssertBuffer(28ull);
    auto readBackBuffer = CreateReadbackBuffer(bufferSizeInBytes);
    auto readBackAllocationBuffer = CreateReadbackBuffer(28ull);

    const auto assertUAV = CreateAssertBufferUAV(assertBuffer, resourceHeap, 0);
    const auto allocationUAV = CreateAssertBufferUAV(allocationBuffer, resourceHeap, 1);

    {
        ScopedDuration testExecution("ShaderTestFixture::RunTest Test Execution");
        const auto capturer = PIXCapturer(InTestDesc.TestName, takeCapture);

        engine.Execute(InTestDesc.TestName,
            [&resourceHeap,
            &pipelineState,
            &rootSignature,
            &assertBuffer,
            &allocationBuffer,
            &readBackBuffer,
            &readBackAllocationBuffer,
            threadGroupCount = InTestDesc.ThreadGroupCount,
            testDataLayout,
            dimX, dimY, dimZ,
            this]
            (ScopedCommandContext& InContext)
            {
                InContext.Section("Test Setup",
                    [&](ScopedCommandContext& InContext)
                    {
                        InContext->SetPipelineState(pipelineState);
                        InContext->SetDescriptorHeaps(resourceHeap);
                        InContext->SetComputeRootSignature(rootSignature);
                        InContext->SetBufferUAV(assertBuffer);
                        InContext->SetBufferUAV(allocationBuffer);

                        const auto assertSection = testDataLayout.GetAssertSection();
                        const auto stringSection = testDataLayout.GetStringSection();
                        const auto sectionSection = testDataLayout.GetSectionInfoSection();
                        std::array params
                        {
                            dimX, dimY, dimZ, 1u,
                            0u, 0u, 0u, 0u,
                            assertSection.Begin(), assertSection.NumMeta(), assertSection.SizeInBytesOfData(), assertSection.SizeInBytesOfSection(),
                            stringSection.Begin(), stringSection.NumMeta(), stringSection.SizeInBytesOfData(), stringSection.SizeInBytesOfSection(),
                            sectionSection.Begin(), sectionSection.NumMeta(), sectionSection.SizeInBytesOfData(), sectionSection.SizeInBytesOfSection()
                        };
                        InContext->SetComputeRoot32BitConstants(0, std::span{ params }, 0);
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
                        InContext->CopyBufferResource(readBackBuffer, assertBuffer);
                        InContext->CopyBufferResource(readBackAllocationBuffer, allocationBuffer);
                    }
                );
            }
        );

        engine.Flush();
    }

    {
        ScopedDuration readbackScope(std::format("ShaderTestFixture::ReadbackResults: {}", InTestDesc.TestName));
        return ReadbackResults(readBackAllocationBuffer, readBackBuffer, uint3(dimX, dimY, dimZ), testDataLayout);
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
    auto commandList = m_Device.CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
    D3D12_COMMAND_QUEUE_DESC queueDesc;
    queueDesc.NodeMask = 0;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    auto commandQueue = m_Device.CreateCommandQueue(queueDesc);

    return CommandEngine(CommandEngine::CreationParams{ std::move(commandList), std::move(commandQueue), m_Device });
}

void ShaderTestFixture::RegisterByteReader(std::string InTypeIDName, STF::MultiTypeByteReader InByteReader)
{
    const u32 typeId = static_cast<u32>(m_ByteReaderMap.size());
    m_Defines.push_back(ShaderMacro{ std::move(InTypeIDName), std::format("{}", typeId) });
    m_ByteReaderMap.push_back(std::move(InByteReader));
}

void ShaderTestFixture::RegisterByteReader(std::string InTypeIDName, STF::SingleTypeByteReader InByteReader)
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
    return m_Device.CreateDescriptorHeap(desc);
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
    return m_Device.CreatePipelineState(desc);
}

RootSignature ShaderTestFixture::CreateRootSignature(const CompiledShaderData& InShaderData) const
{
    return m_Device.CreateRootSignature(InShaderData);
}

GPUResource ShaderTestFixture::CreateAssertBuffer(const u64 InSizeInBytes) const
{
    const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    const auto resourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(InSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

    return m_Device.CreateCommittedResource(heapProps, D3D12_HEAP_FLAG_NONE, resourceDesc, D3D12_BARRIER_LAYOUT_UNDEFINED);
}

GPUResource ShaderTestFixture::CreateReadbackBuffer(const u64 InSizeInBytes) const
{
    const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
    const auto resourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(InSizeInBytes);
    return m_Device.CreateCommittedResource(heapProps, D3D12_HEAP_FLAG_NONE, resourceDesc, D3D12_BARRIER_LAYOUT_UNDEFINED);
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
    m_Device.CreateUnorderedAccessView(InAssertBuffer, uavDesc, uav);
    return uav;
}

STF::Results ShaderTestFixture::ReadbackResults(const GPUResource& InAllocationBuffer, const GPUResource& InAssertBuffer, const uint3 InDispatchDimensions, const STF::TestDataBufferLayout& InTestDataLayout) const
{
    const auto mappedAllocationData = InAllocationBuffer.Map();
    const auto allocationData = mappedAllocationData.Get();

    STF::AllocationBufferData data;

    std::memcpy(&data, allocationData.data(), sizeof(STF::AllocationBufferData));

    const auto mappedAssertData = InAssertBuffer.Map();
    const auto assertData = mappedAssertData.Get();

    return STF::ProcessTestDataBuffer(data, InDispatchDimensions, InTestDataLayout, assertData, m_ByteReaderMap);
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
            return std::format("Undefined Type -> {}", STF::DefaultByteReader(0, InBytes));
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
    return m_Device.IsGPUCaptureEnabled() && takeCaptureIfAble;
}

