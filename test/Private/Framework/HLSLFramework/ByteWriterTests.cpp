#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - ByteWriter")
{
    auto testName = GENERATE
    (
        "GIVEN_FundamentalType_WHEN_HasWriterQueried_THEN_True",
        "GIVEN_FundamentalType_WHEN_BytesRequiredQueried_THEN_ExpectedNumberReturned",
        "GIVEN_FundamentalType_WHEN_AlignmentRequiredQueried_THEN_ExpectedNumberReturned",
        "GIVEN_UIntBufferAndBool_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndInt_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndInt2_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndInt3_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndInt4_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndUInt_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndUInt2_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndUInt3_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndUInt4_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndFloat_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndFloat2_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndFloat3_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndFloat4_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_TypeWithNoWriter_WHEN_HasWriterQueried_THEN_False",
        "GIVEN_TypeWithNoWriter_WHEN_BytesRequiredQueried_THEN_ReturnsSizeOfStruct",
        "GIVEN_TypeWithNoWriter_WHEN_AlignmentRequiredQueried_THEN_ExpectedNumberReturned",
        "GIVEN_TypeWithWriter_WHEN_HasWriterQueried_THEN_True",
        "GIVEN_TypeWithWriter_WHEN_BytesRequiredQueried_THEN_ExpectedNumberReturned",
        "GIVEN_TypeWithWriter_WHEN_AlignmentRequiredQueried_THEN_ExpectedNumberReturned"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/ByteWriter/ByteWriterTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}