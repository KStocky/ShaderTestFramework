
#include "Framework/ShaderTestDriver.h"

#include "Framework/AssertionsV1/TestDataBufferProcessor.h"
#include <d3dx12/d3dx12.h>

namespace stf
{

    static D3D12_UNORDERED_ACCESS_VIEW_DESC CreateRawUAVDesc(const u32 InNumBytes)
    {
        return D3D12_UNORDERED_ACCESS_VIEW_DESC
        {
            .Format = DXGI_FORMAT_R32_TYPELESS,
            .ViewDimension = D3D12_UAV_DIMENSION_BUFFER,
            .Buffer
            {
                .FirstElement = 0,
                .NumElements = (InNumBytes + 3) / 4,
                .StructureByteStride = 0,
                .CounterOffsetInBytes = 0,
                .Flags = D3D12_BUFFER_UAV_FLAG_RAW
            }
        };
    }

    ShaderTestDriver::ShaderTestDriver(ObjectToken InToken, CreationParams InParams)
        : Object(InToken)
        , m_Device(std::move(InParams.Device))
        , m_CommandEngine(Object::New<CommandEngine>(
            CommandEngine::CreationParams
            {
                .Device = m_Device
            }
        ))
    {
    }

    TypeReaderIndex ShaderTestDriver::RegisterByteReader(std::string, MultiTypeByteReader InByteReader)
    {
        const u32 typeId = static_cast<u32>(m_ByteReaderMap.size());
        m_ByteReaderMap.push_back(std::move(InByteReader));

        return TypeReaderIndex{ typeId };
    }

    TypeReaderIndex ShaderTestDriver::RegisterByteReader(std::string InTypeIDName, SingleTypeByteReader InByteReader)
    {
        return RegisterByteReader(std::move(InTypeIDName),
            [byteReader = std::move(InByteReader)](const u16, const std::span<const std::byte> InData)
            {
                return byteReader(InData);
            }
        );
    }
    
    ExpectedError<AssertionsV1::Results> ShaderTestDriver::RunShaderTest(TestDesc&& InTestDesc)
    {
        auto pipelineState = CreatePipelineState(InTestDesc.Shader->GetRootSig(), InTestDesc.Shader->GetCompiledShader());
        const u32 bufferSizeInBytes = std::max(InTestDesc.TestBufferLayout.GetSizeOfTestData(), 4u);
        static constexpr u32 allocationBufferSizeInBytes = sizeof(AssertionsV1::AllocationBufferData);
        const auto dispatchDimensions = InTestDesc.DispatchConfig * InTestDesc.Shader->GetThreadGroupSize();

        struct Resources
        {
            GPUResourceManager::BufferHandle AssertBuffer;
            GPUResourceManager::BufferHandle AllocationBuffer;
            GPUResourceManager::BufferUAVHandle AssertUAV;
            GPUResourceManager::BufferUAVHandle AllocationUAV;
        };

        struct Readbacks
        {
            const GPUResourceManager::ReadbackResultHandle AssertReadback;
            const GPUResourceManager::ReadbackResultHandle AllocationReadback;
        };

        return m_CommandEngine->Execute(InTestDesc.TestName,
            [&](ScopedCommandContext& InContext) -> ExpectedError<Readbacks>
            {
                return InContext.Section("Test Setup",
                    [&](ScopedCommandContext& InContext) -> ExpectedError<Resources>
                    {
                        InContext->SetPipelineState(*pipelineState);

                        const auto assertBuffer = InContext.CreateBuffer(
                            GPUResourceManager::BufferDesc
                            {
                                .Name = "Assert data buffer",
                                .RequestedSize = bufferSizeInBytes,
                                .Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
                            }
                        );

                        const auto allocationBuffer = InContext.CreateBuffer(
                            GPUResourceManager::BufferDesc
                            {
                                .Name = "Allocation data buffer",
                                .RequestedSize = allocationBufferSizeInBytes,
                                .Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
                            }
                        );

                        const auto assertBufferUAV = InContext.CreateUAV(assertBuffer,
                            CreateRawUAVDesc(bufferSizeInBytes));

                        const auto allocationBufferUAV = InContext.CreateUAV(allocationBuffer,
                            CreateRawUAVDesc(allocationBufferSizeInBytes));

                        return Resources
                        {
                            .AssertBuffer = assertBuffer,
                            .AllocationBuffer = allocationBuffer,
                            .AssertUAV = assertBufferUAV,
                            .AllocationUAV = allocationBufferUAV
                        };
                    })
                    .and_then(
                        [&](Resources&& InBuffers) -> ExpectedError<Resources>
                        {
                            return InContext.Section("Test Dispatch",
                                [&](ScopedCommandContext& InContext)
                                {
                                    return InContext.Dispatch(InTestDesc.DispatchConfig, InTestDesc.Shader,
                                        [&](ScopedCommandShader& InShader) -> ExpectedError<void>
                                        {
                                            std::ignore = InShader.StageBindingData(ShaderBinding{ "stf::detail::DispatchDimensions", dispatchDimensions });
                                            std::ignore = InShader.StageBindingData(ShaderBinding{ "stf::detail::Asserts", InTestDesc.TestBufferLayout.GetAssertSection() });
                                            std::ignore = InShader.StageBindingData(ShaderBinding{ "stf::detail::Strings", InTestDesc.TestBufferLayout.GetStringSection() });
                                            std::ignore = InShader.StageBindingData(ShaderBinding{ "stf::detail::Sections", InTestDesc.TestBufferLayout.GetSectionInfoSection() });

                                            std::ignore = InShader.StageBindlessResource("stf::detail::AllocationBufferIndex", InBuffers.AllocationUAV);
                                            std::ignore = InShader.StageBindlessResource("stf::detail::TestDataBufferIndex", InBuffers.AssertUAV);

                                            for (const auto& binding : InTestDesc.Bindings)
                                            {
                                                if (auto result = InShader.StageBindingData(binding); !result)
                                                {
                                                    return result;
                                                }
                                            }

                                            return {};
                                        });
                                })
                                .transform(
                                    [&]() -> Resources
                                    {
                                        return InBuffers;
                                    });
                        })
                    .and_then(
                        [&](const Resources& InBuffers)
                        {
                            return InContext.Section("Results readback",
                                [&](ScopedCommandContext& InContext)
                                {
                                    return InContext.QueueReadback(InBuffers.AssertBuffer)
                                        .and_then(
                                            [&](const GPUResourceManager::ReadbackResultHandle InAssertReadback)
                                            {
                                                return InContext.QueueReadback(InBuffers.AllocationBuffer)
                                                    .transform(
                                                        [&](const GPUResourceManager::ReadbackResultHandle InAllocationReadback)
                                                        {
                                                            return Readbacks
                                                            {
                                                                .AssertReadback = InAssertReadback,
                                                                .AllocationReadback = InAllocationReadback
                                                            };
                                                        });
                                            });
                                });
                        });
            })
            .and_then(
                [&](const Readbacks& InReadbacks)
                {
                    m_CommandEngine->Flush();
                    return m_CommandEngine->ExecuteReadback(InReadbacks.AssertReadback,
                        [&](const MappedResource& InAssertData)
                        {
                            return m_CommandEngine->ExecuteReadback(InReadbacks.AllocationReadback,
                                [&](const MappedResource& InAllocationData) -> ExpectedError<AssertionsV1::Results>
                                {
                                    const auto allocationData = InAllocationData.Get();
                                    AssertionsV1::AllocationBufferData data;
                                    std::memcpy(&data, allocationData.data(), sizeof(AssertionsV1::AllocationBufferData));
                                    const auto assertData = InAssertData.Get();

                                    return ProcessTestDataBuffer(data, dispatchDimensions, InTestDesc.TestBufferLayout, assertData, m_ByteReaderMap);
                                });
                        });
                    
                });
    }
    
    SharedPtr<PipelineState> ShaderTestDriver::CreatePipelineState(const RootSignature& InRootSig, IDxcBlob* InShader) const
    {
        return m_Device->CreatePipelineState(
            D3D12_COMPUTE_PIPELINE_STATE_DESC
            {
                .pRootSignature = InRootSig,
                .CS
                {
                    .pShaderBytecode = InShader->GetBufferPointer(),
                    .BytecodeLength = InShader->GetBufferSize()
                },
                .NodeMask = 0,
                .CachedPSO
                {
                    .pCachedBlob = nullptr,
                    .CachedBlobSizeInBytes = 0
                },
                .Flags = D3D12_PIPELINE_STATE_FLAG_NONE
            });
    }

    AssertionsV1::Results ShaderTestDriver::ReadbackResults(const GPUResource& InAllocationBuffer, const GPUResource& InAssertBuffer, const uint3 InDispatchDimensions, const AssertionsV1::TestDataBufferLayout& InTestDataLayout) const
    {
        const auto mappedAllocationData = InAllocationBuffer.Map();
        const auto allocationData = mappedAllocationData.Get();

        AssertionsV1::AllocationBufferData data;

        std::memcpy(&data, allocationData.data(), sizeof(AssertionsV1::AllocationBufferData));

        const auto mappedAssertData = InAssertBuffer.Map();
        const auto assertData = mappedAssertData.Get();

        return AssertionsV1::ProcessTestDataBuffer(data, InDispatchDimensions, InTestDataLayout, assertData, m_ByteReaderMap);
    }
}
