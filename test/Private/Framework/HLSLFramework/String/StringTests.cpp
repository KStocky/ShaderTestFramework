#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE_FIXTURE(ShaderTestFixtureBaseFixture, "HLSLFrameworkTests - String - String")
{
    const auto testName = GENERATE
    (
        "GIVEN_EmptyString_WHEN_CharacterAppended_THEN_DataIsAsExpected",
        "GIVEN_EmptyString_WHEN_AttemptingToAppendTwoCharactersAtOnce_THEN_OnlyTheFirstIsAppended",
        "GIVEN_EmptyString_WHEN_FourCharactersAppended_THEN_DataIsAsExpected",
        "GIVEN_EmptyString_WHEN_FiveCharactersAppended_THEN_DataIsAsExpected",
        "GIVEN_EmptyStringCreator_WHEN_Executed_THEN_ReturnedStringIsEmpty",
        "GIVEN_NonEmptyStringCreator_WHEN_Executed_THEN_ReturnedStringIsAsExpected",
        "GIVEN_TwoNonEmptyStringCreator_WHEN_Executed_THEN_BothStringsAreAsExpected",
        "GIVEN_FullString_WHEN_AppendCalled_THEN_AppendFails"
    );

    const auto [description, flags] = GENERATE
    (
        table<std::string, std::wstring>
        (
            {
                std::tuple{ "Optimizations Enabled", L"-O3"},
                std::tuple{ "Optimizations Disabled", L"-Od"},
            }
        )
    );

    DYNAMIC_SECTION(description)
    {
        DYNAMIC_SECTION(testName)
        {
            REQUIRE(fixture.RunTest(
                ShaderTestFixture::RuntimeTestDesc
                {
                    .CompilationEnv
                    {
                        .Source = fs::path("/Tests/String/StringTests.hlsl"),
                        .CompilationFlags{ flags },
                    },
                    .TestName = testName,
                    .ThreadGroupCount{1, 1, 1}
                })
            );
        }
    }
}