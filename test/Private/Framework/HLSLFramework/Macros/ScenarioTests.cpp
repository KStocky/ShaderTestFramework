#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - Macros - SCENARIO")
{
    auto testName = GENERATE
    (
        "GIVEN_EmptyScenario_WHEN_Ran_THEN_NoAssertMade",
        "GIVEN_ScenarioWithNoSections_WHEN_Ran_THEN_FunctionOnlyEnteredOnce",
        "GIVEN_SingleSection_WHEN_Ran_THEN_SectionsEnteredOnce",
        "GIVEN_TwoSections_WHEN_Ran_THEN_EachSectionIsEnteredOnce",
        "GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_Ran_THEN_EachSectionIsEnteredOnce",
        "GIVEN_ScenarioWithoutId_WHEN_Ran_THEN_IdIsNone",
        "GIVEN_ScenarioWithDispatchThreadId_WHEN_Ran_THEN_IdIsInt3",
        "GIVEN_ScenarioWithIntId_WHEN_Ran_THEN_IdIsInt"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/Macros/Scenario.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}