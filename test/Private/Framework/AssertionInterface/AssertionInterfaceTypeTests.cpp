
#include "Framework/AssertionInterface.h"

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
    };

    struct ResultsTypeInValid
    {
        using TestRunResultsType = Empty;
    };

    static_assert(CAssertionInterfaceType<ValidType>, "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType<ResultsTypeInValid>, "Expected this type to not be valid for the concept");
}