
#pragma once

#include "Platform.h"

#include "Results.h"
#include "TestDataBufferLayout.h"
#include "Framework/TypeByteReader.h"

#include "D3D12/CommandEngine.h"

#include "D3D12/GPUResourceManager.h"
#include "Utility/Error.h"

#include <string>
#include <vector>

namespace stf::AssertionsV1
{
    class AssertionsV1Interface
    {
    public:

        using PerTestData = TestDataBufferLayoutDesc;
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

        AssertionsV1Interface();

        TypeReaderIndex RegisterByteReader(std::string InTypeIDName, MultiTypeByteReader InByteReader);
        TypeReaderIndex RegisterByteReader(std::string InTypeIDName, SingleTypeByteReader InByteReader);

        std::vector<std::wstring> GetAdditionalCompilerArgs() const;

        ExpectedError<GPUResourcesType> CreateGPUResources(ScopedCommandContext& InContext, const PerTestData& InPerTestData) const;
        ExpectedError<void> BindShaderData(ScopedCommandShader& InShader, const GPUResourcesType& InResources, const PerTestData& InPerTestData) const;
        ExpectedError<GPUReadbackResourcesType> QueueReadbacks(ScopedCommandContext& InContext, const GPUResourcesType& InResources) const;
        ExpectedError<TestRunResultsType> ProcessReadbacks(CommandEngine& InEngine, const GPUReadbackResourcesType& InReadbacks, const PerTestData& InPerTestData) const;

    private:

        MultiTypeByteReaderMap m_ByteReaderMap;
        std::vector<std::wstring> m_AdditionalArgs;
    };
}
