#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE_FIXTURE(ShaderTestFixtureBaseFixture, "HLSLFrameworkTests - Macros - SECTIONS")
{
    auto testName = GENERATE
    (
        "GIVEN_EmptySection_WHEN_RanUsingSingle_THEN_NoAssertMade",
        "GIVEN_SingleSection_WHEN_RanUsingSingle_THEN_SectionsEnteredOnce",
        "GIVEN_TwoSections_WHEN_RanUsingSingle_THEN_EachSectionIsEnteredOnce",
        "GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_RanUsingSingle_THEN_EachSectionIsEnteredOnce",
        "GIVEN_TwoSubSectionsWithTwoNestedSubsections_WHEN_RanUsingSingle_THEN_ExpectedSubsectionEntryOccurs",
        "GIVEN_ThreeLevelsDeepSections_WHEN_RanUsingSingle_THEN_ExpectedSubsectionEntryOccurs"
    );

    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(
            ShaderTestFixture::RuntimeTestDesc
            {
                .CompilationEnv
                {
                    .Source = fs::path("/Tests/Macros/Sections.hlsl")
                },
                .TestName = testName,
                .ThreadGroupCount{1, 1, 1}
            }
        ));
    }
}