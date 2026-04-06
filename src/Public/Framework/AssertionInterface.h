#pragma once

#include "Platform.h"
#include "Utility/Concepts.h"

namespace stf
{
    class ScopedCommandContext;
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
        requires(T In, ScopedCommandContext& InContext)
        {
            { In.CreateGPUResources(InContext) } -> std::same_as<typename T::GPUResourcesType>;
        };
}