
#include "Framework/AssertionsV1/AssertionsV1Interface.h"
#include "Framework/AssertionsV1/TestDataBufferProcessor.h"

namespace stf::AssertionsV1
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

    AssertionsV1Interface::AssertionsV1Interface(const CreationParams& InParams)
        : m_Params(InParams)
    {
    }

    ExpectedError<AssertionsV1Interface::GPUResourcesType> AssertionsV1Interface::CreateGPUResources(ScopedCommandContext& InContext) const
    {
        const u32 bufferSizeInBytes = std::max(m_Params.GetSizeOfTestData(), 4u);
        static constexpr u32 allocationBufferSizeInBytes = sizeof(AssertionsV1::AllocationBufferData);

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

        return GPUResourcesType
        {
            .AssertBuffer = assertBuffer,
            .AllocationBuffer = allocationBuffer,
            .AssertUAV = assertBufferUAV,
            .AllocationUAV = allocationBufferUAV
        };
    }

    ExpectedError<void> AssertionsV1Interface::BindShaderData(ScopedCommandShader& InShader, const GPUResourcesType& InResources) const
    {
        const auto dispatchDimensions = InShader.GetThreadCount();
        std::ignore = InShader.StageBindingData(ShaderBinding{ "stf::AssertionsV1::detail::DispatchDimensions", dispatchDimensions });
        std::ignore = InShader.StageBindingData(ShaderBinding{ "stf::AssertionsV1::detail::Asserts", m_Params.GetAssertSection() });
        std::ignore = InShader.StageBindingData(ShaderBinding{ "stf::AssertionsV1::detail::Strings", m_Params.GetStringSection() });
        std::ignore = InShader.StageBindingData(ShaderBinding{ "stf::AssertionsV1::detail::Sections", m_Params.GetSectionInfoSection() });

        std::ignore = InShader.StageBindlessResource("stf::AssertionsV1::detail::AllocationBufferIndex", InResources.AllocationUAV);
        std::ignore = InShader.StageBindlessResource("stf::AssertionsV1::detail::TestDataBufferIndex", InResources.AssertUAV);

        return {};
    }

    ExpectedError<AssertionsV1Interface::GPUReadbackResourcesType> AssertionsV1Interface::QueueReadbacks( ScopedCommandContext& InContext, const GPUResourcesType& InResources) const
    {
        return InContext.QueueReadback(InResources.AssertBuffer)
            .and_then(
                [&](const GPUResourceManager::ReadbackResultHandle InAssertReadback)
                {
                    return InContext.QueueReadback(InResources.AllocationBuffer)
                        .transform(
                            [&](const GPUResourceManager::ReadbackResultHandle InAllocationReadback)
                            {
                                return GPUReadbackResourcesType
                                {
                                    .AssertReadback = InAssertReadback,
                                    .AllocationReadback = InAllocationReadback
                                };
                            });
                });
    }

    ExpectedError<AssertionsV1Interface::TestRunResultsType> AssertionsV1Interface::ProcessReadbacks(CommandEngine& InEngine, const GPUReadbackResourcesType& InReadbacks) const
    {
        return InEngine.ExecuteReadback(InReadbacks.AssertReadback,
            [&](const MappedResource& InAssertData)
            {
                return InEngine.ExecuteReadback(InReadbacks.AllocationReadback,
                    [&](const MappedResource& InAllocationData) -> ExpectedError<TestRunResultsType>
                    {
                        const auto allocationData = InAllocationData.Get();
                        AssertionsV1::AllocationBufferData data;
                        std::memcpy(&data, allocationData.data(), sizeof(AssertionsV1::AllocationBufferData));
                        const auto assertData = InAssertData.Get();

                        return ProcessTestDataBuffer(data, m_Params, assertData, m_ByteReaderMap);
                    });
            });
    }
}
