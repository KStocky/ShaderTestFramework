#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE_PERSISTENT_FIXTURE(ShaderTestFixtureBaseFixture, "HLSLFrameworkTests - AssertBuffer - SizeTests")
{
    auto [testName, numRecordedFailedAsserts, numBytesAssertData] = GENERATE
    (
        table<std::string, u32, u32>
        (
            {
                std::tuple{"GIVEN_ZeroAssertsRecorded_WHEN_Ran_THEN_AssertBufferInfoAsExpected", 0, 0},
                std::tuple{"GIVEN_FiveAssertsRecordedAndNoData_WHEN_Ran_THEN_AssertBufferInfoAsExpected", 5, 0},
                std::tuple{"GIVEN_FiveAssertsRecordedAnd200BytesOfData_WHEN_Ran_THEN_AssertBufferInfoAsExpected", 5, 200},
                std::tuple{"GIVEN_FiveAssertsRecordedAndNonMultipleOf8BytesOfData_WHEN_Ran_THEN_AssertBufferInfoAsExpected", 5, 97}
            }
        )
    );

    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(
            ShaderTestFixture::RuntimeTestDesc
            {
                .CompilationEnv
                {
                    .Source = fs::path("/Tests/TestDataBuffer/SizeTests.hlsl")
                },
                .TestName = testName,
                .ThreadGroupCount{1, 1, 1},
                .TestDataLayout
                {
                    .NumFailedAsserts = numRecordedFailedAsserts,
                    .NumBytesAssertData = numBytesAssertData
                }
            }
        ));
    }
}