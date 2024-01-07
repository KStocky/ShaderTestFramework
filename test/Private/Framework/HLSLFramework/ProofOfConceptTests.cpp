#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - ProofOfConcept")
{
    auto testName = GENERATE
    (
        "GIVEN_TwoCallsToCounter_WHEN_Compared_THEN_AreDifferent",
        "GIVEN_StaticGlobalArray_WHEN_Inspected_THEN_AllZeroed",
        "SectionTest",
        "GIVEN_TwoDifferentSizedStructs_WHEN_sizeofCalledOn_Them_THEN_CorrectSizeReported",
        "GIVEN_SomeTypesWithAndWithoutASpecializations_WHEN_ApplyFuncCalledOnThem_THEN_ExpectedResultsReturned",
        "VariadicMacroOverloading"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path(std::format("/Tests/ProofOfConcept/{}.hlsl", testName))));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}