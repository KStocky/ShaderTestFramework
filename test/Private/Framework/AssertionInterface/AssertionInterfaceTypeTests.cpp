
#include "Framework/AssertionInterface.h"

#include "D3D12/CommandEngine.h"
#include "TestUtilities/UniqueType.h"

namespace stf::assert::AssertionInterfaceTypeTests
{
    struct Empty {};

    struct ValidTestRunResultsType
    {
        bool Succeeded() const { return true; }
        friend bool operator==(const ValidTestRunResultsType&, const ValidTestRunResultsType&) { return false; };
        friend bool operator!=(const ValidTestRunResultsType&, const ValidTestRunResultsType&) { return true; };

        friend std::ostream& operator<<(std::ostream& InOut, const ValidTestRunResultsType&)
        {
            return InOut;
        }
    };

    struct ValidType
    {
        using TestRunResultsType = ValidTestRunResultsType;
        using GPUResourcesType = UniqueType<Empty>;

        GPUResourcesType CreateGPUResources(ScopedCommandContext&)
        {
            return GPUResourcesType{};
        }
    };

    struct InvalidResultsType
    {
        using TestRunResultsType = UniqueType<Empty>;

        using GPUResourcesType = UniqueType<Empty>;

        GPUResourcesType CreateGPUResources(ScopedCommandContext&)
        {
            return GPUResourcesType{};
        }
    };

    struct CreateGPUResourcesReturnsWrongType
    {
        using TestRunResultsType = ValidTestRunResultsType;
        using GPUResourcesType = UniqueType<Empty>;

        auto CreateGPUResources(ScopedCommandContext&)
        {
            return UniqueType<Empty>{};
        }
    };

    struct CreateGPUResourcesAcceptsByConstRef
    {
        using TestRunResultsType = ValidTestRunResultsType;
        using GPUResourcesType = UniqueType<Empty>;

        GPUResourcesType CreateGPUResources(const ScopedCommandContext&)
        {
            return GPUResourcesType{};
        }
    };

    struct CreateGPUResourcesAcceptsByValue
    {
        using TestRunResultsType = ValidTestRunResultsType;
        using GPUResourcesType = UniqueType<Empty>;

        GPUResourcesType CreateGPUResources(ScopedCommandContext)
        {
            return GPUResourcesType{};
        }
    };

    struct CreateGPUResourcesAcceptsByRValueRef
    {
        using TestRunResultsType = ValidTestRunResultsType;
        using GPUResourcesType = UniqueType<Empty>;

        GPUResourcesType CreateGPUResources(ScopedCommandContext&&)
        {
            return GPUResourcesType{};
        }
    };

    static_assert(CAssertionInterfaceType<ValidType>, "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType<InvalidResultsType>, "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType<CreateGPUResourcesReturnsWrongType>, "Expected this type to not be valid for the concept");
    static_assert(CAssertionInterfaceType<CreateGPUResourcesAcceptsByConstRef>, "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType<CreateGPUResourcesAcceptsByValue>, "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType<CreateGPUResourcesAcceptsByRValueRef>, "Expected this type to not be valid for the concept");
}