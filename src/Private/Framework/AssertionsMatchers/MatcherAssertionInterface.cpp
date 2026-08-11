
#include "Framework/AssertionsMatchers/MatcherAssertionInterface.h"

#include <format>
#include <ranges>
#include <sstream>

namespace stf::Matchers
{

    TypeReaderIndex AssertionsMatchersInterface::RegisterByteReader([[maybe_unused]] std::string InTypeIDName, [[maybe_unused]] MultiTypeByteReader InByteReader)
    {
        return TypeReaderIndex{ 0 };
    }

    TypeReaderIndex AssertionsMatchersInterface::RegisterByteReader([[maybe_unused]] std::string InTypeIDName, [[maybe_unused]] SingleTypeByteReader InByteReader)
    {
        return TypeReaderIndex{ 0 };
    }

    std::vector<std::wstring> AssertionsMatchersInterface::GetAdditionalCompilerArgs() const
    {
        return {};
    }

    ExpectedError<AssertionsMatchersInterface::GPUResourcesType> AssertionsMatchersInterface::CreateGPUResources([[maybe_unused]] ScopedCommandContext& InContext, [[maybe_unused]] const PerTestData& InPerTestData) const
    {
        return Unexpected{ Error::FromFragment<"Not Implemented">() };
    }

    ExpectedError<void> AssertionsMatchersInterface::BindShaderData([[maybe_unused]] ScopedCommandShader& InShader, [[maybe_unused]] const GPUResourcesType& InResources, [[maybe_unused]] const PerTestData& InPerTestData) const
    {
        return Unexpected{ Error::FromFragment<"Not Implemented">() };
    }

    ExpectedError<AssertionsMatchersInterface::GPUReadbackResourcesType> AssertionsMatchersInterface::QueueReadbacks([[maybe_unused]] ScopedCommandContext& InContext, [[maybe_unused]] const GPUResourcesType& InResources) const
    {
        return Unexpected{ Error::FromFragment<"Not Implemented">() };
    }

    ExpectedError<AssertionsMatchersInterface::TestRunResultsType> AssertionsMatchersInterface::ProcessReadbacks([[maybe_unused]] CommandEngine& InEngine, [[maybe_unused]] const GPUReadbackResourcesType& InReadbacks, [[maybe_unused]] const PerTestData& InPerTestData) const
    {
        return Unexpected{ Error::FromFragment<"Not Implemented">() };
    }
}
