#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - AssertBuffer - SizeTests")
{
    auto [testName, numRecordedFailedAsserts, numBytesAssertData] = GENERATE
    (
        table<std::string, u32, u32>
        (
            {
                std::tuple{"GIVEN_ZeroAssertsRecorded_WHEN_Ran_THEN_AssertBufferInfoAsExpected", 0, 0},
                std::tuple{"GIVEN_FiveAssertsRecordedAndNoData_WHEN_Ran_THEN_AssertBufferInfoAsExpected", 5, 0},
                std::tuple{"GIVEN_FiveAssertsRecordedAnd100BytesOfData_WHEN_Ran_THEN_AssertBufferInfoAsExpected", 5, 100},
                std::tuple{"GIVEN_FiveAssertsRecordedAndNonMultipleOf4BytesOfData_WHEN_Ran_THEN_AssertBufferInfoAsExpected", 5, 97}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/AssertBuffer/SizeTests.hlsl"), {numRecordedFailedAsserts, numBytesAssertData}));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}