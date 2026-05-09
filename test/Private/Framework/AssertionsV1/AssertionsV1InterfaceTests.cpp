
#include "Framework/AssertionInterface.h"
#include "Framework/AssertionsV1/AssertionsV1Interface.h"

namespace stf::AssertionsV1::AssertionsV1InterfaceTests
{
    static_assert(stf::assert::CTestRunResultsType<TestRunResults>,
        "AssertionsV1::TestRunResults must satisfy the CTestRunResultsType concept");

    static_assert(stf::assert::CAssertionInterfaceType<AssertionsV1Interface>,
        "AssertionsV1Interface must satisfy the CAssertionInterfaceType concept");
}
