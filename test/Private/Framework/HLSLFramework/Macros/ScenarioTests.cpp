#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/AssertionsV1/ShaderTestFixture.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE_PERSISTENT_FIXTURE(ShaderTestFixtureBaseFixture, "HLSLFrameworkTests - Macros - SCENARIO")
{
    using namespace stf;
    auto testName = GENERATE
    (
        "GIVEN_EmptyScenario_WHEN_Ran_THEN_NoAssertMade",
        "GIVEN_ScenarioWithNoSections_WHEN_Ran_THEN_FunctionOnlyEnteredOnce",
        "GIVEN_SingleSection_WHEN_Ran_THEN_SectionsEnteredOnce",
        "GIVEN_TwoSections_WHEN_Ran_THEN_EachSectionIsEnteredOnce",
        "GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_Ran_THEN_EachSectionIsEnteredOnce",
        "GIVEN_ScenarioWithoutId_WHEN_Ran_THEN_IdIsNone",
        "GIVEN_ScenarioWithDispatchThreadId_WHEN_Ran_THEN_IdAsExpected"
    );

    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(
            AssertionsV1::ShaderTestFixture::RuntimeTestDesc
            {
                .CompilationEnv
                {
                    .Source = fs::path("/Tests/Macros/Scenario.hlsl")
                },
                .TestName = testName,
                .ThreadGroupCount{1, 1, 1}
            }
        ));
    }
}