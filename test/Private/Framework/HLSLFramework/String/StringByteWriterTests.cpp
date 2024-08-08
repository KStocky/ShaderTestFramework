#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE_PERSISTENT_FIXTURE(ShaderTestFixtureBaseFixture, "HLSLFrameworkTests - String - StringByteWriter")
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

    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(
            ShaderTestFixture::RuntimeTestDesc
            {
                .CompilationEnv
                {
                    .Source = fs::path("/Tests/String/StringByteWriterTests.hlsl")
                },
                .TestName = testName,
                .ThreadGroupCount{1, 1, 1}
            }
        ));
    }
}