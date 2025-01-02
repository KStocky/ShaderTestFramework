
#include "Framework/ShaderTestDriver.h"
#include "Framework/ShaderTestCommon.h"
#include "Framework/TestDataBufferProcessor.h"

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

    ShaderTestDriver::ShaderTestDriver(CreationParams InParams)
        : m_Device(std::move(InParams.Device))
        , m_CommandEngine(MakeShared<CommandEngine>(
            CommandEngine::CreationParams
            {
                .Device = m_Device
            }
        ))
        , m_DescriptorManager(MakeShared<ShaderTestDescriptorManager>(
            ShaderTestDescriptorManager::CreationParams{
                .Device = m_Device,
                .InitialSize = 16
            }
        ))
    {
    }

    SharedPtr<GPUResource> ShaderTestDriver::CreateBuffer(const D3D12_HEAP_TYPE InType, const D3D12_RESOURCE_DESC1& InDesc)
    {
        const auto heapProps = CD3DX12_HEAP_PROPERTIES(InType);

        return m_Device->CreateCommittedResource(heapProps, D3D12_HEAP_FLAG_NONE, InDesc, D3D12_BARRIER_LAYOUT_UNDEFINED);
    }

    ShaderTestUAV ShaderTestDriver::CreateUAV(SharedPtr<GPUResource> InResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc)
    {
        return ThrowIfUnexpected(m_DescriptorManager->CreateUAV(InResource, InDesc)
            .or_else(
                [&, this](const ShaderTestDescriptorManager::EErrorType InErrorType) -> Expected<ShaderTestUAV, ShaderTestDescriptorManager::EErrorType>
                {
                    switch (InErrorType)
                    {
                        case ShaderTestDescriptorManager::EErrorType::AllocatorFull:
                        {
                            m_DeferredDeletedDescriptorHeaps.push_back(
                                ThrowIfUnexpected(m_DescriptorManager->Resize(m_DescriptorManager->GetCapacity() * 2)));
                            return m_DescriptorManager->CreateUAV(InResource, InDesc);
                        }
                    }

                    return Unexpected{ InErrorType };
                }
            ));
    }

    TypeReaderIndex ShaderTestDriver::RegisterByteReader(std::string InTypeIDName, MultiTypeByteReader InByteReader)
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

    Results ShaderTestDriver::RunShaderTest(const ShaderTestShader& InShader, const TestDataBufferLayout& InTestBufferLayout, const std::string_view InTestName, const uint3 InDispatchConfig)
    {
        auto pipelineState = CreatePipelineState(*InShader.GetRootSig(), InShader.GetCompiledShader());
        const u32 bufferSizeInBytes = std::max(InTestBufferLayout.GetSizeOfTestData(), 4u);
        static constexpr u32 allocationBufferSizeInBytes = sizeof(AllocationBufferData);
        auto assertBuffer = CreateBuffer(D3D12_HEAP_TYPE_DEFAULT, CD3DX12_RESOURCE_DESC1::Buffer(bufferSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS));
        auto allocationBuffer = CreateBuffer(D3D12_HEAP_TYPE_DEFAULT, CD3DX12_RESOURCE_DESC1::Buffer(allocationBufferSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS));
        auto readBackBuffer = CreateBuffer(D3D12_HEAP_TYPE_READBACK, CD3DX12_RESOURCE_DESC1::Buffer(bufferSizeInBytes));
        auto readBackAllocationBuffer = CreateBuffer(D3D12_HEAP_TYPE_READBACK, CD3DX12_RESOURCE_DESC1::Buffer(allocationBufferSizeInBytes));;

        const auto assertUAV = CreateUAV(assertBuffer, CreateRawUAVDesc(bufferSizeInBytes));
        const auto allocationUAV = CreateUAV(allocationBuffer, CreateRawUAVDesc(allocationBufferSizeInBytes));;

        {
            m_CommandEngine->Execute(InTestName,
                [this,
                &pipelineState,
                &assertBuffer,
                &allocationBuffer,
                &readBackBuffer,
                &readBackAllocationBuffer,
                InDispatchConfig,
                &InShader
                ]
                (ScopedCommandContext& InContext)
                {
                    InContext.Section("Test Setup",
                        [&](ScopedCommandContext& InContext)
                        {
                            InContext->SetPipelineState(*pipelineState);
                            m_DescriptorManager->SetDescriptorHeap(*InContext);
                            InContext->SetComputeRootSignature(*InShader.GetRootSig());
                            InContext->SetBufferUAV(*assertBuffer);
                            InContext->SetBufferUAV(*allocationBuffer);

                            InShader.SetConstantBufferData(*InContext);
                        }
                    );

                    InContext.Section("Test Dispatch",
                        [InDispatchConfig](ScopedCommandContext& InContext)
                        {
                            InContext->Dispatch(InDispatchConfig.x, InDispatchConfig.y, InDispatchConfig.z);
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

            m_CommandEngine->Flush();
            m_DeferredDeletedDescriptorHeaps.clear();
        }

        {
            return ReadbackResults(*readBackAllocationBuffer, *readBackBuffer, InShader.GetThreadGroupSize() * InDispatchConfig, InTestBufferLayout);
        }
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

    Results ShaderTestDriver::ReadbackResults(const GPUResource& InAllocationBuffer, const GPUResource& InAssertBuffer, const uint3 InDispatchDimensions, const TestDataBufferLayout& InTestDataLayout) const
    {
        const auto mappedAllocationData = InAllocationBuffer.Map();
        const auto allocationData = mappedAllocationData.Get();

        AllocationBufferData data;

        std::memcpy(&data, allocationData.data(), sizeof(AllocationBufferData));

        const auto mappedAssertData = InAssertBuffer.Map();
        const auto assertData = mappedAssertData.Get();

        return ProcessTestDataBuffer(data, InDispatchDimensions, InTestDataLayout, assertData, m_ByteReaderMap);
    }
}
