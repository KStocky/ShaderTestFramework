
#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <sstream>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

SCENARIO("ShaderTestFixtureTests")
{
    GIVEN("Default Constructed Fixture")
    {
        ShaderTestFixture test{ {} };
        WHEN("Queried for Validity")
        {
            THEN("Is Valid")
            {
                REQUIRE(test.IsValid());
            }
        }

        WHEN("Queried for AgilitySDK")
        {
            THEN("Has Agility")
            {
                REQUIRE(test.IsUsingAgilitySDK());
            }
        }
    }
}

SCENARIO("HLSLFrameworkTests - AssertBuffer - ResultProcessing - NoAssertBuffer")
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

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/AssertBuffer/ResultsProcessing/NoAssertBuffer.hlsl"), { 0, 0 }));
    DYNAMIC_SECTION(testName)
    {
        const auto results = fixture.RunTest(testName, 1, 1, 1);
        CAPTURE(results);
        const auto actual = results.GetTestResults();
        REQUIRE(actual);
        REQUIRE(*actual == expected);
    }
}

SCENARIO("HLSLFrameworkTests - AssertBuffer - ResultProcessing - AssertInfoWithNoData")
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
                std::vector{STF::FailedAssert{{}, {}, STF::AssertMetaData{64, 0, 0}}}, 0, 1, uint3(1,1,1) },
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

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/AssertBuffer/ResultsProcessing/AssertInfoWithNoData.hlsl"), { numRecordedAsserts, 0 }));

    DYNAMIC_SECTION(testName)
    {
        const auto results = fixture.RunTest(testName, 1, 1, 1);
        CAPTURE(results);
        const auto actual = results.GetTestResults();
        REQUIRE(actual);
        REQUIRE(*actual == expected);
    }
}

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

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/AssertBuffer/ResultsProcessing/AssertInfoWithData.hlsl"), { numRecordedAsserts, numBytesData }));
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

SCENARIO("HLSLFrameworkTests - AssertBuffer - ResultProcessing - TypeConverter")
{
    using Catch::Matchers::ContainsSubstring;

    auto [testName, expectedSubstrings] = GENERATE
    (
        table<std::string, std::vector<const char*>>
        (
            {
                std::tuple
                {
                    "GIVEN_FailedSingleAssert_WHEN_NoTypeId_THEN_HasExpectedResults",
                    std::vector{"Undefined"}
                },
                std::tuple
                {
                    "GIVEN_FailedSingleAssert_WHEN_TypeId_THEN_HasExpectedResults",
                    std::vector{"TYPE 1", "12345678"}
                },
                std::tuple
                {
                    "GIVEN_FailedTwoSingleAsserts_WHEN_FirstNoTypeIdSecondHasTypeId_THEN_HasExpectedResults",
                    std::vector{"TYPE 1", "12345678", "Undefined"}
                },
                std::tuple
                {
                    "GIVEN_FailedTwoSingleAsserts_WHEN_BothHaveSameTypeId_THEN_HasExpectedResults",
                    std::vector{"TYPE 1", "12345678", "1234"}
                },
                std::tuple
                {
                    "GIVEN_FailedTwoSingleAsserts_WHEN_BothHaveDifferentTypeId_THEN_HasExpectedResults",
                    std::vector{"TYPE 1", "12345678", "TYPE 2", "87654321"}
                }
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/AssertBuffer/ResultsProcessing/TypeConverter.hlsl"), { 10, 400 }));
    fixture.RegisterTypeConverter("TEST_TYPE_1", 
        [](const std::span<const std::byte> InBytes) 
        {
            u32 value;
            std::memcpy(&value, InBytes.data(), sizeof(u32));
            return std::format("TYPE 1: {}", value); 
        });
    fixture.RegisterTypeConverter("TEST_TYPE_2",
        [](const std::span<const std::byte> InBytes)
        {
            u32 value;
            std::memcpy(&value, InBytes.data(), sizeof(u32));
            return std::format("TYPE 2: {}", value);
        });

    DYNAMIC_SECTION(testName)
    {
        const auto results = fixture.RunTest(testName, 1, 1, 1);
        CAPTURE(results);
        const auto actual = results.GetTestResults();
        REQUIRE(actual);

        std::stringstream stream;
        stream << *actual;

        for (const auto expectedString : expectedSubstrings)
        {
            REQUIRE_THAT(stream.str(), ContainsSubstring(expectedString, Catch::CaseSensitive::No));
        }
    }
}

SCENARIO("HLSLFrameworkTests - ByteWriter")
{
    auto testName = GENERATE
    (
        "GIVEN_FundamentalType_WHEN_HasWriterQueried_THEN_True",
        "GIVEN_FundamentalType_WHEN_BytesRequiredQueried_THEN_ExpectedNumberReturned",
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
        "GIVEN_NonFundamentalTypeWithNoWriter_WHEN_HasWriterQueried_THEN_False",
        "GIVEN_NonFundamentalTypeWithNoWriter_WHEN_BytesRequiredQueried_THEN_Zero",
        "GIVEN_NonFundamentalTypeWithWriter_WHEN_HasWriterQueried_THEN_True",
        "GIVEN_NonFundamentalTypeWithWriter_WHEN_BytesRequiredQueried_THEN_ExpectedNumberReturned"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/ByteWriter/ByteWriterTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - Cast")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_StructDoesNotHaveOverloadOfCast_WHEN_CastCalled_THEN_Fails", false},
                std::tuple{"GIVEN_StructDoesHaveOverloadOfCast_WHEN_CastCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_StructDoesHasDifferentOverloadOfCast_WHEN_CastCalled_THEN_Fails", false}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path(std::format("/Tests/HLSLFrameworkTests/Cast/{}.hlsl", testName))));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = fixture.RunTest(testName, 1, 1, 1);
            REQUIRE_FALSE(result);
        }
    }
}

SCENARIO("HLSLFrameworkTests - Asserts - AreEqual")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_TwoEqualInts_WHEN_AreEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoNotEqualInts_WHEN_AreEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoEqualFloat4_WHEN_AreEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoNotEqualFloat4_WHEN_AreEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoEqualNamedInts_WHEN_AreEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoNotEqualNamedInts_WHEN_AreEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoEqualNamedFloat4_WHEN_AreEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoNotEqualNamedFloat4_WHEN_AreEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoObjectsWithEqualOperatorOverloads_WHEN_ObjectsAreEqual_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoObjectsWithEqualOperatorOverloads_WHEN_ObjectsAreNotEqual_THEN_Fails", false}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Asserts/AreEqual.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = fixture.RunTest(testName, 1, 1, 1);
            REQUIRE_FALSE(result);
        }
    }
}

SCENARIO("HLSLFrameworkTests - Asserts - NotEqual")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_TwoEqualInts_WHEN_NotEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoNotEqualInts_WHEN_NotEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoEqualFloat4_WHEN_NotEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoNotEqualFloat4_WHEN_NotEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoEqualNamedInts_WHEN_NotEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoNotEqualNamedInts_WHEN_NotEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoEqualNamedFloat4_WHEN_NotEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoNotEqualNamedFloat4_WHEN_NotEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoObjectsWithNotEqualOperatorOverloads_WHEN_ObjectsAreEqual_THEN_Fails", false},
                std::tuple{"GIVEN_TwoObjectsWithNotEqualOperatorOverloads_WHEN_ObjectsAreNotEqual_THEN_Succeeds", true}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Asserts/NotEqual.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = fixture.RunTest(testName, 1, 1, 1);
            REQUIRE_FALSE(result);
        }
    }
}

SCENARIO("HLSLFrameworkTests - Asserts - IsTrue")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_TrueLiteral_WHEN_IsTrueCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TrueNonLiteral_WHEN_IsTrueCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TrueExpression_WHEN_IsTrueCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsTrue_THEN_Succeeds", true},
                std::tuple{"GIVEN_FalseLiteral_WHEN_IsTrueCalled_THEN_Fails", false},
                std::tuple{"GIVEN_FalseNonLiteral_WHEN_IsTrueCalled_THEN_Fails", false},
                std::tuple{"GIVEN_FalseExpression_WHEN_IsTrueCalled_THEN_Fails", false},
                std::tuple{"GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsFalse_THEN_Fails", false}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Asserts/IsTrue.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = fixture.RunTest(testName, 1, 1, 1);
            REQUIRE_FALSE(result);
        }
    }
}

SCENARIO("HLSLFrameworkTests - Asserts - Fail")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_TestWithFailAssert_WHEN_Ran_THEN_Fails", false},
                std::tuple{"GIVEN_Empty_WHEN_Ran_THEN_Succeeds", true}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Asserts/Fail.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = fixture.RunTest(testName, 1, 1, 1);
            REQUIRE_FALSE(result);
        }
    }
}

SCENARIO("HLSLFrameworkTests - Asserts - IsFalse")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_TrueLiteral_WHEN_IsFalseCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TrueNonLiteral_WHEN_IsFalseCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TrueExpression_WHEN_IsFalseCalled_THEN_Fails", false},
                std::tuple{"GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsTrue_THEN_Fails", false},
                std::tuple{"GIVEN_FalseLiteral_WHEN_IsFalseCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_FalseNonLiteral_WHEN_IsFalseCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_FalseExpression_WHEN_IsFalseCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsFalse_THEN_Succeeds", true}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Asserts/IsFalse.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = fixture.RunTest(testName, 1, 1, 1);
            REQUIRE_FALSE(result);
        }
    }
}

SCENARIO("HLSLFrameworkTests - Macros - AssertMacro")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_TwoEqualInts_WHEN_AreEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoNotEqualInts_WHEN_AreEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoEqualInts_WHEN_NotEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoNotEqualInts_WHEN_NotEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TrueLiteral_WHEN_IsTrueCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_FalseLiteral_WHEN_IsTrueCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TrueLiteral_WHEN_IsFalseCalled_THEN_Fails", false},
                std::tuple{"GIVEN_FalseLiteral_WHEN_IsFalseCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TestWithFailAssert_WHEN_Ran_THEN_Fails", false}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Macros/AssertMacro.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = fixture.RunTest(testName, 1, 1, 1);
            REQUIRE_FALSE(result);
        }
    }
}

SCENARIO("HLSLFrameworkTests - Macros - NumArgs")
{
    auto testName = GENERATE
    (
        "GIVEN_ZeroArgs_WHEN_Counted_THEN_ReturnsZero",
        "GIVEN_OneArg_WHEN_Counted_THEN_ReturnsOne",
        "GIVEN_TenArgs_WHEN_Counted_THEN_ReturnsTen"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Macros/NumArgs.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - Macros - SectionVarCreation")
{
    auto testName = GENERATE
    (
        "GIVEN_SingleSectionVarCreated_WHEN_Queried_THEN_ValueIsZero",
        "GIVEN_TwoSectionVarsCreated_WHEN_Queried_THEN_ValueAreAsExpected",
        "GIVEN_TwoSectionVarsCreatedInALoop_WHEN_Queried_THEN_ValueAreAsExpected"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Macros/SectionVarCreation.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}


SCENARIO("HLSLFrameworkTests - Macros - SCENARIO")
{
    auto testName = GENERATE
    (
        "GIVEN_EmptyScenario_WHEN_Ran_THEN_NoAssertMade",
        "GIVEN_ScenarioWithNoSections_WHEN_Ran_THEN_FunctionOnlyEnteredOnce",
        "GIVEN_SingleSection_WHEN_Ran_THEN_SectionsEnteredOnce",
        "GIVEN_TwoSections_WHEN_Ran_THEN_EachSectionIsEnteredOnce",
        "GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_Ran_THEN_EachSectionIsEnteredOnce",
        "GIVEN_ScenarioWithoutId_WHEN_Ran_THEN_IdIsNone",
        "GIVEN_ScenarioWithDispatchThreadId_WHEN_Ran_THEN_IdIsInt3",
        "GIVEN_ScenarioWithIntId_WHEN_Ran_THEN_IdIsInt"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Macros/Scenario.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - Macros - SECTIONS")
{
    auto testName = GENERATE
    (
        "GIVEN_EmptySection_WHEN_RanUsingBeginEnd_THEN_NoAssertMade",
        "GIVEN_EmptySection_WHEN_RanUsingSingle_THEN_NoAssertMade",
        "GIVEN_SingleSection_WHEN_RanUsingBeginEnd_THEN_SectionsEnteredOnce",
        "GIVEN_SingleSection_WHEN_RanUsingSingle_THEN_SectionsEnteredOnce",
        "GIVEN_TwoSections_WHEN_RanUsingBeginEnd_THEN_EachSectionIsEnteredOnce",
        "GIVEN_TwoSections_WHEN_RanUsingSingle_THEN_EachSectionIsEnteredOnce",
        "GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_RanUsingBeginEnd_THEN_EachSectionIsEnteredOnce",
        "GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_RanUsingSingle_THEN_EachSectionIsEnteredOnce",
        "GIVEN_TwoSubSectionsWithTwoNestedSubsections_WHEN_RanUsingBeginEnd_THEN_ExpectedSubsectionEntryOccurs",
        "GIVEN_TwoSubSectionsWithTwoNestedSubsections_WHEN_RanUsingSingle_THEN_ExpectedSubsectionEntryOccurs",
        "GIVEN_ThreeLevelsDeepSections_WHEN_RanUsingBeginEnd_THEN_ExpectedSubsectionEntryOccurs",
        "GIVEN_ThreeLevelsDeepSections_WHEN_RanUsingSingle_THEN_ExpectedSubsectionEntryOccurs"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Macros/Sections.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - ProofOfConcept")
{
    auto testName = GENERATE
    (
        "GIVEN_TwoCallsToCounter_WHEN_Compared_THEN_AreDifferent",
        "GIVEN_StaticGlobalArray_WHEN_Inspected_THEN_AllZeroed",
        "SectionTest",
        "GIVEN_TwoDifferentSizedStructs_WHEN_sizeofCalledOn_Them_THEN_CorrectSizeReported",
        "GIVEN_SomeTypesWithAndWithoutASpecializations_WHEN_ApplyFuncCalledOnThem_THEN_ExpectedResultsReturned",
        "VariadicMacroOverloading"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path(std::format("/Tests/HLSLFrameworkTests/ProofOfConcept/{}.hlsl", testName))));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - Container_wrapper")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_IntArray_WHEN_PropertiesQueried_THEN_AsExpected", true},
                std::tuple{"GIVEN_IntArray_WHEN_LoadCalled_THEN_ReturnsExpectedValue", true},
                std::tuple{"GIVEN_IntArray_WHEN_StoreCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_IntArray_WHEN_StoreCalledWithDifferentType_THEN_Fails", false },
                std::tuple{"GIVEN_IntArray_WHEN_Store2Called_THEN_Succeeds", true},
                std::tuple{"GIVEN_IntArray_WHEN_Store2CalledWithDifferentType_THEN_Fails", false },
                std::tuple{"GIVEN_IntArray_WHEN_Store3Called_THEN_Succeeds", true},
                std::tuple{"GIVEN_IntArray_WHEN_Store3CalledWithDifferentType_THEN_Fails", false },
                std::tuple{"GIVEN_IntArray_WHEN_Store4Called_THEN_Succeeds", true},
                std::tuple{"GIVEN_IntArray_WHEN_Store4CalledWithDifferentType_THEN_Fails", false }
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path(std::format("/Tests/HLSLFrameworkTests/Container/ArrayTests/{}.hlsl", testName))));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = fixture.RunTest(testName, 1, 1, 1);
            REQUIRE_FALSE(result);
        }
    }
}

SCENARIO("HLSLFrameworkTests - ThreadDimensionTests")
{
    auto [testName, dimX, dimY, dimZ] = GENERATE
    (
        table<std::string, u32, u32, u32>
        (
            {
                std::tuple{"GIVEN_SingleThreadDispatched_WHEN_DispatchDimensionsQueried_THEN_IsAsExpected", 1, 1, 1},
                std::tuple{"GIVEN_SingleThreadPerGroupAnd10Groups_WHEN_DispatchDimensionsQueried_THEN_IsAsExpected", 10, 10, 10},
                std::tuple{"GIVEN_SingleGroupWithGroupSizeOf10_WHEN_DispatchDimensionsQueried_THEN_IsAsExpected", 1, 1, 1},
                std::tuple{"GIVEN_GroupWithSide2WithGroupSizeOfSide2_WHEN_DispatchDimensionsQueried_THEN_IsAsExpected", 2, 2, 2}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/ThreadDimensionTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, dimX, dimY, dimZ));
    }
}

SCENARIO("HLSLFrameworkTests - ThreadIdRegistrationTests")
{
    auto [testName, dimX, dimY, dimZ] = GENERATE
    (
        table<std::string, u32, u32, u32>
        (
            {
                std::tuple{"GIVEN_SingleThreadDispatched_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect", 1, 1, 1},
                std::tuple{"GIVEN_SingleThreadPerGroupAnd10Groups_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect", 10, 10, 10},
                std::tuple{"GIVEN_SingleGroupWithGroupSizeOf10_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect", 1, 1, 1},
                std::tuple{"GIVEN_GroupWithSide2WithGroupSizeOfSide2_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect", 2, 2, 2},
                std::tuple{"GIVEN_ThreadIDGivenAsUint_WHEN_ThreadIDQueried_THEN_RegisteredThreadIdIsCorrect", 1, 1, 1},
                std::tuple{"GIVEN_ThreadIdNotRegistered_WHEN_ThreadIDQueried_THEN_RegisteredThreadIdIsCorrect", 1, 1, 1}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/ThreadIdRegistrationTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, dimX, dimY, dimZ));
    }
}

SCENARIO("HLSLFrameworkTests - TypeTraits - ArrayTraits")
{
    auto testName = GENERATE
    (
        "GIVEN_BuiltInArray_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_NonArray_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_VectorType_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_Buffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_StructuredBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWStructuredBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_ByteAddressBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWByteAddressBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/TypeTraits/ArrayTraitsTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - TypeTraits - ContainerTraits")
{
    auto testName = GENERATE
    (
        "GIVEN_BuiltInArray_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_NonArray_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_VectorType_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_Buffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_StructuredBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWStructuredBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_ByteAddressBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWByteAddressBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/TypeTraits/ContainerTraitsTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - TypeTraits - FundamentalTypeTraits")
{
    auto testName = GENERATE
    (
        "AllShouldPass"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/TypeTraits/FundamentalTypeTraitsTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - TypeTraits - FundamentalType")
{
    auto testName = GENERATE
    (
        "AllShouldPass"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/TypeTraits/FundamentalTypeTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - TypeTraits - TypeID")
{
    auto testName = GENERATE
    (
        "NoDuplicateTypeIDs"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/TypeTraits/TypeIDTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - SectionManagement")
{
    auto testName = GENERATE
    (
        "GIVEN_SingleSection_WHEN_Ran_THEN_SectionsEnteredOnce",
        "GIVEN_SingleSubsection_WHEN_RanUsingIf_THEN_SectionsEntered2Times",
        "GIVEN_TwoSubsections_WHEN_RanUsingIf_THEN_SectionsEntered4Times",
        "GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_RanUsingIf_THEN_SectionsEntered5Times",
        "GIVEN_SingleSubsection_WHEN_RanUsingWhile_THEN_SectionsEntered2Times",
        "GIVEN_TwoSubsections_WHEN_RanUsingWhile_THEN_SectionsEntered4Times",
        "GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_RanUsingWhile_THEN_SectionsEntered5Times"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/SectionManagement.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - FlattenIndex")
{
    auto testName = GENERATE
    (
        "GIVEN_UnitCubeAndZeroIndex_WHEN_Flattened_THEN_ReturnsZero",
        "GIVEN_CubeSide10AndZeroIndex_WHEN_Flattened_THEN_ReturnsZero",
        "GIVEN_CubeSide10AndIndicesDifferentByOneInX_WHEN_BothFlattened_THEN_DifferenceIsOne",
        "GIVEN_CubeSide10AndIndicesDifferentByOneInY_WHEN_BothFlattened_THEN_DifferenceIsTen",
        "GIVEN_CubeSide10AndIndicesDifferentByOneInZ_WHEN_BothFlattened_THEN_DifferenceIs100",
        "GIVEN_CubeSide3_WHEN_IndicesIncrementedInXThenYThenZ_AND_WHEN_Flattened_THEN_DifferenceIs1"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/FlattenIndex.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - Bugs")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                //std::tuple
                //{
                //    "GIVEN_WarpAndNoOptimizations_WHEN_ThisIsRun_THEN_ItWillSomeTimesFail",
                //    true
                //},
                std::tuple
                {
                    "GIVEN_Object_WHEN_ConversionOperatorCalled_THEN_Fails",
                    false
                }
            }
        )
    );


    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path(std::format("/Tests/HLSLFrameworkTests/Bugs/{}.hlsl", testName))));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = fixture.RunTest(testName, 1, 1, 1);
            REQUIRE_FALSE(result);
        }
    }
}