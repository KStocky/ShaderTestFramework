#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - TestDataBuffer - ResultProcessing - AssertInfoWithNoData")
{
    auto [testName, numRecordedAsserts, failedAsserts, numSucceeded, numFailed, dims] = GENERATE
    (
        table<std::string, u32, std::vector<STF::FailedAssert>, u32 , u32, uint3>
        (
            {
                std::tuple{ "GIVEN_AssertInfoCapacity_WHEN_ZeroAssertsMade_THEN_HasExpectedResults", 10,
                std::vector<STF::FailedAssert>{}, 0, 0, uint3(1,1,1) },
                std::tuple{ "GIVEN_AssertInfoCapacity_WHEN_NonZeroSuccessfulAssertsMade_THEN_HasExpectedResults", 10,
                std::vector<STF::FailedAssert>{}, 2, 0, uint3(1,1,1) },
                std::tuple{ "GIVEN_AssertInfoCapacity_WHEN_FailedAssertNoTypeId_THEN_HasExpectedResults", 10,
                std::vector{STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0 }}}, 0, 1, uint3(1,1,1) },
                std::tuple{ "GIVEN_AssertInfoCapacity_WHEN_TwoFailedAssert_THEN_HasExpectedResults", 10,
                std::vector{STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}},
                STF::FailedAssert{{}, {}, STF::AssertMetaData{32, 0, 0}}}, 0, 2, uint3(1,1,1) },
                std::tuple{ "GIVEN_AssertInfoCapacity_WHEN_FailedAssertWithLineId_THEN_HasExpectedResults", 10,
                std::vector{STF::FailedAssert{{}, {}, STF::AssertMetaData{59, 0, 0}}}, 0, 1, uint3(1,1,1) },
                std::tuple{ "GIVEN_AssertInfoCapacityWithFlatThreadId_WHEN_FailedAssert_THEN_HasExpectedResults", 10,
                std::vector{STF::FailedAssert{{}, {}, STF::AssertMetaData{66, 12, 1}}}, 0, 1, uint3(1,1,1) },
                std::tuple{ "GIVEN_AssertInfoCapacityWithFlat3DThreadId_WHEN_FailedAssert_THEN_HasExpectedResults", 10,
                std::vector{STF::FailedAssert{{}, {}, STF::AssertMetaData{66, 12, 2}}}, 0, 1, uint3(24,1,1) },
                std::tuple{ "GIVEN_AssertInfoCapacityWithNonFlat3DThreadId_WHEN_FailedAssert_THEN_HasExpectedResults", 10,
                std::vector{STF::FailedAssert{{}, {}, STF::AssertMetaData{66, 4, 2}}}, 0, 1, uint3(3,3,3) },
                std::tuple{ "GIVEN_AssertInfoCapacity_WHEN_MoreFailedAssertsThanCapacity_THEN_HasExpectedResults", 1,
                std::vector{STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0 }}}, 0, 2, uint3(1,1,1) },
            }
        )
    );

    const STF::TestRunResults expected
    {
        .FailedAsserts = std::move(failedAsserts),
        .NumSucceeded = numSucceeded,
        .NumFailed = numFailed,
        .DispatchDimensions = dims
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
                    .Source = fs::path("/Tests/TestDataBuffer/ResultsProcessing/AssertInfoWithNoData.hlsl")
                },
                .TestName = testName,
                .ThreadGroupCount{1, 1, 1},
                .TestDataLayout
                {
                    .NumFailedAsserts = numRecordedAsserts,
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