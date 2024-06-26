#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - String - StringByteWriter")
{
    auto testName = GENERATE
    (
        "GIVEN_EmptyString_WHEN_BytesRequiredQueried_THEN_ZeroReturned",
        "GIVEN_StringWithMultipleOf4Characters_WHEN_BytesRequiredQueried_THEN_BytesRequiredIsSameAsNumberOfChars",
        "GIVEN_StringWithNonMultipleOf4Characters_WHEN_BytesRequiredQueried_THEN_BytesRequiredIsNextMultipleOf4",
        "GIVEN_String_WHEN_AlignmentRequiredQueried_THEN_FourReturned",
        "GIVEN_EmptyString_WHEN_WriteCalled_THEN_NothingWritten",
        "GIVEN_StringWith4Characters_WHEN_WriteCalled_THEN_4CharactersWritten",
        "GIVEN_StringWith5Characters_WHEN_WriteCalled_THEN_5CharactersWritten"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/String/StringByteWriterTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}