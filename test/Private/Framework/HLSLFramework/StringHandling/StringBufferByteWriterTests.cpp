#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - StringHandling - StringBufferByteWriter")
{
    auto testName = GENERATE
    (
        "GIVEN_EmptyStringBuffer_WHEN_BytesRequiredQueried_THEN_ZeroReturned",
        "GIVEN_StringBufferWithMultipleOf4Characters_WHEN_BytesRequiredQueried_THEN_BytesRequiredIsSameAsNumberOfChars",
        "GIVEN_StringBufferWithNonMultipleOf4Characters_WHEN_BytesRequiredQueried_THEN_BytesRequiredIsNextMultipleOf4",
        "GIVEN_StringBuffer_WHEN_AlignmentRequiredQueried_THEN_FourReturned",
        "GIVEN_EmptyStringBuffer_WHEN_WriteCalled_THEN_NothingWritten",
        "GIVEN_StringBufferWith4Characters_WHEN_WriteCalled_THEN_4CharactersWritten",
        "GIVEN_StringBufferWith5Characters_WHEN_WriteCalled_THEN_5CharactersWritten"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/StringHandling/StringBufferByteWriterTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}