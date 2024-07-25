#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - TestDataBuffer - ResultProcessing - NoAssertBuffer")
{
    auto [testName, numSucceeded, numFailed] = GENERATE
    (
        table<std::string, u32, u32>
        (
            {
                std::tuple{ "GIVEN_ZeroAssertBuffer_WHEN_ZeroAssertsMade_THEN_HasExpectedResults", 0, 0},
                std::tuple{ "GIVEN_ZeroAssertBuffer_WHEN_NonZeroSuccessfulAssertsMade_THEN_HasExpectedResults", 2, 0 },
                std::tuple{ "GIVEN_ZeroAssertBuffer_WHEN_NonZeroFailedAssertsMade_THEN_HasExpectedResults", 0, 2 },
                std::tuple{ "GIVEN_ZeroAssertBuffer_WHEN_NonZeroSuccessfulAndFailedAssertsMade_THEN_HasExpectedResults", 2, 2 },
            }
        )
    );

    const STF::TestRunResults expected
    {
        .FailedAsserts = {},
        .NumSucceeded = numSucceeded,
        .NumFailed = numFailed,
        .DispatchDimensions = uint3(1,1,1)
    };

    ShaderTestFixture fixture(
        ShaderTestFixture::FixtureDesc
        {
            .Mappings{ GetTestVirtualDirectoryMapping() }
        }
    );

    DYNAMIC_SECTION(testName)
    {
        const auto results = fixture.RunTest(
            ShaderTestFixture::RuntimeTestDesc
            {
                .CompilationEnv
                {
                    .Source = fs::path("/Tests/TestDataBuffer/ResultsProcessing/NoAssertBuffer.hlsl")
                },
                .TestName = testName,
                .ThreadGroupCount{1, 1, 1},
                .TestDataLayout
                {
                    .NumFailedAsserts = 0,
                    .NumBytesAssertData = 0
                }
            }
        );
        CAPTURE(results);
        const auto actual = results.GetTestResults();
        REQUIRE(actual);
        REQUIRE(*actual == expected);
    }
}