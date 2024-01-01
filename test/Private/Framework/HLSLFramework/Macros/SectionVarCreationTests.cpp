#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - Macros - SectionVarCreation")
{
    auto testName = GENERATE
    (
        "GIVEN_SingleSectionVarCreated_WHEN_Queried_THEN_ValueIsZero",
        "GIVEN_TwoSectionVarsCreated_WHEN_Queried_THEN_ValueAreAsExpected",
        "GIVEN_TwoSectionVarsCreatedInALoop_WHEN_Queried_THEN_ValueAreAsExpected"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Macros/SectionVarCreation.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}