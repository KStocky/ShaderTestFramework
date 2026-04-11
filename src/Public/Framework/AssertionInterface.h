#pragma once

#include "Platform.h"
#include "Utility/Concepts.h"
#include "Utility/Error.h"

namespace stf
{
    class ScopedCommandContext;
    class ScopedCommandShader;
}

namespace stf::assert
{
    template<typename T>
    concept CTestRunResultsType = 
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
            const typename T::CreationParams& InParams,
            const typename T::GPUResourcesType& InResources,
            ScopedCommandContext& InContext,
            ScopedCommandShader& InShader)
        {
            T{ InParams };
            { In.CreateGPUResources(InContext) } -> std::same_as<ExpectedError<typename T::GPUResourcesType>>;
            { In.BindShaderData(InShader, InResources) } -> std::same_as<ExpectedError<void>>;
            { In.QueueReadbacks(InResources) } -> std::same_as<ExpectedError<typename T::GPUReadbackResourcesType>>;
        };
}