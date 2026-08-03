
#include "Framework/AssertionInterface.h"

namespace stf::assert::TestRunResultsTypeTests
{
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

    struct NonConstSucceeded
    {
        bool Succeeded() { return true; }
        friend bool operator==(const NonConstSucceeded&, const NonConstSucceeded&) { return false; };
        friend bool operator!=(const NonConstSucceeded&, const NonConstSucceeded&) { return true; };

        friend std::ostream& operator<<(std::ostream& InOut, const NonConstSucceeded&)
        {
            return InOut;
        }
    };

    struct NoSucceeded
    {
        friend bool operator==(const NoSucceeded&, const NoSucceeded&) { return false; };
        friend bool operator!=(const NoSucceeded&, const NoSucceeded&) { return true; };

        friend std::ostream& operator<<(std::ostream& InOut, const NoSucceeded&)
        {
            return InOut;
        }
    };

    struct NoOperatorEquals
    {
        bool Succeeded() const { return true; }
        friend bool operator!=(const NoOperatorEquals&, const NoOperatorEquals&) { return true; };

        friend std::ostream& operator<<(std::ostream& InOut, const NoOperatorEquals&)
        {
            return InOut;
        }
    };

    struct NoOStreamOperator
    {
        bool Succeeded() const { return true; }
        friend bool operator==(const NoOStreamOperator&, const NoOStreamOperator&) { return false; };
        friend bool operator!=(const NoOStreamOperator&, const NoOStreamOperator&) { return true; };
    };

    struct NoDefaultCtor
    {
        NoDefaultCtor() = delete;
        bool Succeeded() const { return true; }
        friend bool operator==(const NoDefaultCtor&, const NoDefaultCtor&) { return false; };
        friend bool operator!=(const NoDefaultCtor&, const NoDefaultCtor&) { return true; };

        friend std::ostream& operator<<(std::ostream& InOut, const NoDefaultCtor&)
        {
            return InOut;
        }
    };

    static_assert(CTestRunResultsType<ValidTestRunResultsType>, "Expected this type to be valid for the concept");
    static_assert(!CTestRunResultsType<NonConstSucceeded>, "Expected this type to not be valid for the concept");
    static_assert(!CTestRunResultsType<NoSucceeded>, "Expected this type to not be valid for the concept");
    static_assert(!CTestRunResultsType<NoOperatorEquals>, "Expected this type to not be valid for the concept");
    static_assert(!CTestRunResultsType<NoOStreamOperator>, "Expected this type to not be valid for the concept");
    static_assert(!CTestRunResultsType<NoDefaultCtor>, "Expected this type to not be valid for the concept");
}