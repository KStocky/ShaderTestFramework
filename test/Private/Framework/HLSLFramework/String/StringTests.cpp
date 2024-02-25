#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - String - String")
{
    auto testName = GENERATE
    (
        "GIVEN_EmptyString_WHEN_CharacterAppended_THEN_DataIsAsExpected",
        "GIVEN_EmptyString_WHEN_AttemptingToAppendTwoCharactersAtOnce_THEN_OnlyTheFirstIsAppended",
        "GIVEN_EmptyString_WHEN_FourCharactersAppended_THEN_DataIsAsExpected",
        "GIVEN_EmptyString_WHEN_FiveCharactersAppended_THEN_DataIsAsExpected",
        "GIVEN_FullString_WHEN_AppendCharCalled_THEN_AppendFails",
        "StringLambdaConstructionTest"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/String/StringTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}