#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - AssertBuffer - ResultProcessing - AssertInfoWithData")
{

    auto serializeImpl = []<typename T>(const T& InVal, std::vector<std::byte>& InOutBytes)
    {
        static constexpr u32 size = sizeof(T);
        const auto oldSize = InOutBytes.size();
        InOutBytes.resize(InOutBytes.size() + sizeof(u32) + sizeof(T));
        std::memcpy(InOutBytes.data() + oldSize, &size, sizeof(u32));
        std::memcpy(InOutBytes.data() + oldSize + sizeof(u32), &InVal, size);
    };

    auto serialize = [&serializeImpl]<typename... T>(const T&... InVals)
    {
        std::vector<std::byte> ret;
        (serializeImpl(InVals, ret), ...);
        return ret;
    };

    auto test = serialize(32u, 1024u);

    static constexpr u32 expectedValueLeft = 34u;
    static constexpr u32 expectedValueRight = 12345678u;
    static constexpr uint2 expectedExtra{ 123u, 42u };

    auto [testName, expected, numRecordedAsserts, numBytesData] = GENERATE_COPY
    (
        table<std::string, STF::TestRunResults, u32, u32>
        (
            {
                std::tuple
                { 
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedSingleAssertWithoutTypeIdOrWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, 0, 1, uint3(1,1,1)},
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedSingleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(expectedValueLeft), {}, STF::AssertMetaData{42, 0, 0}}}, 0, 1, uint3(1,1,1)},
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedSingleAssertWithTypeIdNoWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, 0, 1, uint3(1,1,1)},
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedSingleAssertWithTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(expectedValueLeft), {}, STF::AssertMetaData{42, 0, 0}}}, 0, 1, uint3(1,1,1)},
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedDoubleAssertWithoutTypeIdOrWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, 0, 1, uint3(1,1,1)},
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedDoubleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(expectedValueLeft, expectedValueRight), {}, STF::AssertMetaData{42, 0, 0}}}, 0, 1, uint3(1,1,1)},
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedDoubleAssertWithTypeIdNoWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, 0, 1, uint3(1,1,1)},
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedDoubleAssertWithTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(expectedValueLeft, expectedValueRight), {}, STF::AssertMetaData{42, 0, 0}}}, 0, 1, uint3(1,1,1)},
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndNotEnoughDataCapacity_WHEN_FailedSingleAssertWithoutTypeIdOrWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, 0, 1, uint3(1,1,1)},
                    10, 4
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndNotEnoughDataCapacity_WHEN_FailedSingleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, 0, 1, uint3(1,1,1)},
                    10, 4
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndNotEnoughDataCapacity_WHEN_FailedSingleAssertWithTypeIdNoWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, 0, 1, uint3(1,1,1)},
                    10, 4
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndNotEnoughDataCapacity_WHEN_FailedSingleAssertWithTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, 0, 1, uint3(1,1,1)},
                    10, 4
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_LargeFailFirstThenSmallFailSingleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}, STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, 0, 2, uint3(1,1,1)},
                    10, 12
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_LargeFailFirstThenSmallFailSingleAssertWithTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}, STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, 0, 2, uint3(1,1,1)},
                    10, 12
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_SmallFailFirstThenLargeFailSingleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(expectedValueRight), {}, STF::AssertMetaData{42, 0, 0}}, STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, 0, 2, uint3(1,1,1)},
                    10, 12
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_SmallFailFirstThenLargeFailSingleAssertWithTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(expectedValueRight), {}, STF::AssertMetaData{42, 0, 0}}, STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, 0, 2, uint3(1,1,1)},
                    10, 12
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_SmallStructComparedWithLargerStructWithWriterAndEnoughCapacity_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(1u, uint3(1u, 2u, 3u)), {}, STF::AssertMetaData{42, 0, 0}}}, 0, 1, uint3(1,1,1)},
                    10, 100
                },
                    std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_LargeStructComparedWithSmallerStructWithWriterAndEnoughCapacity_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(uint3(1u, 2u, 3u), 1u), {}, STF::AssertMetaData{42, 0, 0}}}, 0, 1, uint3(1,1,1)},
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_OneLargeFailDoubleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(uint3(1000, 2000, 3000), uint3(4000, 5000, 6000)), {}, STF::AssertMetaData{42, 0, 0}}}, 0, 1, uint3(1,1,1)},
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoLargeFailDoubleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(uint3(1000, 2000, 3000), uint3(4000, 5000, 6000)), {}, STF::AssertMetaData{42, 0, 0}}, STF::FailedAssert{serialize(uint3(1000, 2000, 3000), uint3(4000, 5000, 6000)), {}, STF::AssertMetaData{42, 0, 0}}}, 0, 2, uint3(1,1,1)},
                    10, 100
                }
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/AssertBuffer/ResultsProcessing/AssertInfoWithData.hlsl"), { numRecordedAsserts, numBytesData }));
    fixture.RegisterTypeConverter("TEST_TYPE_WITH_WRITER", [](const std::span<const std::byte>) { return ""; });

    DYNAMIC_SECTION(testName)
    {
        const auto results = fixture.RunTest(testName, 1, 1, 1);
        CAPTURE(results);
        const auto actual = results.GetTestResults();
        REQUIRE(actual);
        REQUIRE(*actual == expected);
    }
}