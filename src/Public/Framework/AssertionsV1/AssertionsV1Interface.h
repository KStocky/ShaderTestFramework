
#pragma once

#include "Platform.h"

#include "Results.h"
#include "TestDataBufferLayout.h"
#include "Framework/TypeByteReader.h"

#include "D3D12/CommandEngine.h"

#include "D3D12/GPUResourceManager.h"
#include "Utility/Error.h"

namespace stf::AssertionsV1
{
    class AssertionsV1Interface
    {
    public:

        using CreationParams = TestDataBufferLayout;
        using TestRunResultsType = TestRunResults;
        
        struct GPUResourcesType
        {
            const GPUResourceManager::BufferHandle AssertBuffer;
            const GPUResourceManager::BufferHandle AllocationBuffer;
            const GPUResourceManager::BufferUAVHandle AssertUAV;
            const GPUResourceManager::BufferUAVHandle AllocationUAV;
        };

        struct GPUReadbackResourcesType
        {
            const GPUResourceManager::ReadbackResultHandle AssertReadback;
            const GPUResourceManager::ReadbackResultHandle AllocationReadback;
        };

        AssertionsV1Interface(const CreationParams& InParams);

        ExpectedError<GPUResourcesType> CreateGPUResources(ScopedCommandContext& InContext) const;
        ExpectedError<void> BindShaderData(ScopedCommandShader& InShader, const GPUResourcesType& InResources) const;
        ExpectedError<GPUReadbackResourcesType> QueueReadbacks(ScopedCommandContext& InContext, const GPUResourcesType& InResources) const;
        ExpectedError<TestRunResultsType> ProcessReadbacks(CommandEngine& InEngine, const GPUReadbackResourcesType& InReadbacks) const;

    private:

        CreationParams m_Params;
        MultiTypeByteReaderMap m_ByteReaderMap;
    };
}
