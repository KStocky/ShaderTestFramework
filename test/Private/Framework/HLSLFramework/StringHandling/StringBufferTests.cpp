#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - StringHandling - StringBuffer")
{
    auto testName = GENERATE
    (
        "GIVEN_EmptyStringBuffer_WHEN_CharacterAppended_THEN_DataIsAsExpected",
        "GIVEN_EmptyStringBuffer_WHEN_AttemptingToAppendTwoCharactersAtOnce_THEN_OnlyTheFirstIsAppended",
        "GIVEN_EmptyStringBuffer_WHEN_FourCharactersAppended_THEN_DataIsAsExpected",
        "GIVEN_EmptyStringBuffer_WHEN_FiveCharactersAppended_THEN_DataIsAsExpected",
        "GIVEN_FullStringBuffer_WHEN_AppendCharCalled_THEN_AppendFails"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/StringHandling/StringBufferTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}