#pragma once

#include "Platform.h"
#include "Utility/Concepts.h"
#include "Utility/Error.h"

#include <string>
#include <vector>

namespace stf
{
    class CommandEngine;
    class ScopedCommandContext;
    class ScopedCommandShader;
}

namespace stf::assert
{
    template<typename T>
    concept CTestRunResultsType = 
        std::default_initializable<T> &&
        OStreamable<T> &&
        std::equality_comparable<T> &&
        requires(const T& In)
        {
            { In.Succeeded() } -> std::same_as<bool>;
        };

    template<typename T>
    concept CAssertionInterfaceType =
        CTestRunResultsType<typename T::TestRunResultsType> &&
        requires(
            T In,
            const T& InConst,
            const typename T::PerTestData& InPerTestData,
            const typename T::GPUResourcesType& InResources,
            const typename T::GPUReadbackResourcesType& InReadbacks,
            ScopedCommandContext& InContext,
            ScopedCommandShader& InShader,
            CommandEngine& InEngine)
        {
            { InConst.GetAdditionalCompilerArgs() } -> std::same_as<std::vector<std::wstring>>;
            { In.CreateGPUResources(InContext, InPerTestData) } -> std::same_as<ExpectedError<typename T::GPUResourcesType>>;
            { In.BindShaderData(InShader, InResources, InPerTestData) } -> std::same_as<ExpectedError<void>>;
            { In.QueueReadbacks(InContext, InResources) } -> std::same_as<ExpectedError<typename T::GPUReadbackResourcesType>>;
            { In.ProcessReadbacks(InEngine, InReadbacks, InPerTestData) } -> std::same_as<ExpectedError<typename T::TestRunResultsType>>;
        };
}