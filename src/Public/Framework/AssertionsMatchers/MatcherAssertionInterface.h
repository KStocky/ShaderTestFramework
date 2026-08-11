
#pragma once

#include "Platform.h"

#include "Framework/AssertionsV1/Results.h"
#include "Framework/AssertionsV1/TestDataBufferLayout.h"

#include "D3D12/CommandEngine.h"
#include "D3D12/GPUResourceManager.h"

#include "Framework/TypeByteReader.h"

#include "Utility/Error.h"

#include <string>
#include <vector>

namespace stf::Matchers
{
    class AssertionsMatchersInterface
    {
    public:

        using PerTestData = AssertionsV1::TestDataBufferLayoutDesc;
        using TestRunResultsType = AssertionsV1::TestRunResults;

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

        AssertionsMatchersInterface() = default;

        TypeReaderIndex RegisterByteReader(std::string InTypeIDName, MultiTypeByteReader InByteReader);
        TypeReaderIndex RegisterByteReader(std::string InTypeIDName, SingleTypeByteReader InByteReader);

        std::vector<std::wstring> GetAdditionalCompilerArgs() const;

        ExpectedError<GPUResourcesType> CreateGPUResources(ScopedCommandContext& InContext, const PerTestData& InPerTestData) const;
        ExpectedError<void> BindShaderData(ScopedCommandShader& InShader, const GPUResourcesType& InResources, const PerTestData& InPerTestData) const;
        ExpectedError<GPUReadbackResourcesType> QueueReadbacks(ScopedCommandContext& InContext, const GPUResourcesType& InResources) const;
        ExpectedError<TestRunResultsType> ProcessReadbacks(CommandEngine& InEngine, const GPUReadbackResourcesType& InReadbacks, const PerTestData& InPerTestData) const;

        static constexpr StringLiteral AssertionLibraryVirtualPath{ "/Test/STF/AssertionsV1/Framework.hlsli" };

    private:

        MultiTypeByteReaderMap m_ByteReaderMap;
        std::vector<std::wstring> m_AdditionalArgs;
    };
}
