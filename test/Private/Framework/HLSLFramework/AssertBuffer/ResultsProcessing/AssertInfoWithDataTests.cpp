#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Utility/Math.h>
#include <Utility/OverloadSet.h>
#include <Utility/Tuple.h>
#include <Utility/TypeTraits.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - AssertBuffer - ResultProcessing - AssertInfoWithData")
{

    auto serializeImpl = OverloadSet{ 
        [] <typename T>(const T& InVal, std::vector<std::byte>& InOutBytes) -> std::enable_if_t<!TIsInstantiationOf<Tuple, T>::Value>
        {
            static constexpr u32 size = sizeof(T);
            static constexpr u32 align = alignof(T);
            static constexpr u32 sizeAndAlign = (size << 16) | align;
            static constexpr u64 sizeOfAllocation = AlignedOffset(size + 4, 8);

            const auto oldSize = InOutBytes.size();

            InOutBytes.resize(InOutBytes.size() + sizeOfAllocation);
            auto address = InOutBytes.data() + oldSize;
            std::memcpy(address, &sizeAndAlign, sizeof(u32));
            address += AlignedOffset(sizeof(u32), align);
            std::memcpy(address, &InVal, size);
        },
        [] <typename T>(const Tuple<T, T>& InVal, std::vector<std::byte>& InOutBytes)
        {
            static constexpr u32 size = sizeof(T);
            static constexpr u32 align = alignof(T);
            static constexpr u32 sizeAndAlign = (size << 16) | align;
            static constexpr u64 sizeSingle = AlignedOffset(size + 4, align);
            static constexpr u64 sizeOfAllocation = AlignedOffset(sizeSingle * 2, 8);

            const auto oldSize = InOutBytes.size();

            InOutBytes.resize(InOutBytes.size() + sizeOfAllocation);
            auto address = InOutBytes.data() + oldSize;
            std::memcpy(address, &sizeAndAlign, sizeof(u32));
            address += AlignedOffset(sizeof(u32), align);
            std::memcpy(address, &Get<0>(InVal), size);
            address += AlignedOffset(size, 4);
            std::memcpy(address, &sizeAndAlign, sizeof(u32));
            address += AlignedOffset(sizeof(u32), align);
            std::memcpy(address, &Get<1>(InVal), size);
        }
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

    auto [testName, expected, numRecordedAsserts, numBytesData] = GENERATE_COPY
    (
        table<std::string, STF::TestRunResults, u32, u32>
        (
            {
                std::tuple
                { 
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedSingleAssertWithoutTypeIdOrWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(expectedValueLeft), {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 1, uint3(1,1,1)},
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedSingleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(expectedValueLeft), {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 1, uint3(1,1,1)},
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedSingleAssertWithTypeIdNoWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(expectedValueLeft), {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 1, uint3(1,1,1)},
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedSingleAssertWithTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(expectedValueLeft), {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 1, uint3(1,1,1)},
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedDoubleAssertWithoutTypeIdOrWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(expectedValueLeft, expectedValueRight), {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 1, uint3(1,1,1)},
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedDoubleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(expectedValueLeft, expectedValueRight), {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 1, uint3(1,1,1)},
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedDoubleAssertWithTypeIdNoWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(expectedValueLeft, expectedValueRight), {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 1, uint3(1,1,1)},
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedDoubleAssertWithTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(expectedValueLeft, expectedValueRight), {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 1, uint3(1,1,1)},
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndNotEnoughDataCapacity_WHEN_FailedSingleAssertWithoutTypeIdOrWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 1, uint3(1,1,1)},
                    10, 4
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndNotEnoughDataCapacity_WHEN_FailedSingleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 1, uint3(1,1,1)},
                    10, 4
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndNotEnoughDataCapacity_WHEN_FailedSingleAssertWithTypeIdNoWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 1, uint3(1,1,1)},
                    10, 4
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndNotEnoughDataCapacity_WHEN_FailedSingleAssertWithTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 1, uint3(1,1,1)},
                    10, 4
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_LargeFailFirstThenSmallFailSingleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}, STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 2, uint3(1,1,1)},
                    10, 12
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_LargeFailFirstThenSmallFailSingleAssertWithTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}, STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 2, uint3(1,1,1)},
                    10, 12
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_SmallFailFirstThenLargeFailSingleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(expectedValueRight), {}, STF::AssertMetaData{42, 0, 0}}, STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 2, uint3(1,1,1)},
                    10, 12
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_SmallFailFirstThenLargeFailSingleAssertWithTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(expectedValueRight), {}, STF::AssertMetaData{42, 0, 0}}, STF::FailedAssert{{}, {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 2, uint3(1,1,1)},
                    10, 12
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_SmallStructComparedWithLargerStructWithWriterAndEnoughCapacity_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(1u, uint3(1u, 2u, 3u)), {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 1, uint3(1,1,1)},
                    10, 100
                },
                    std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_LargeStructComparedWithSmallerStructWithWriterAndEnoughCapacity_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(uint3(1u, 2u, 3u), 1u), {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 1, uint3(1,1,1)},
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_OneLargeFailDoubleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(uint3(1000, 2000, 3000), uint3(4000, 5000, 6000)), {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 1, uint3(1,1,1)},
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoLargeFailDoubleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(uint3(1000, 2000, 3000), uint3(4000, 5000, 6000)), {}, STF::AssertMetaData{42, 0, 0}}, STF::FailedAssert{serialize(uint3(1000, 2000, 3000), uint3(4000, 5000, 6000)), {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 2, uint3(1,1,1)},
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoLargeFailDoubleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(uint3(1000, 2000, 3000), uint3(4000, 5000, 6000)), {}, STF::AssertMetaData{42, 0, 0}}, STF::FailedAssert{serialize(uint3(1000, 2000, 3000), uint3(4000, 5000, 6000)), {}, STF::AssertMetaData{42, 0, 0}}}, {}, 0, 2, uint3(1,1,1)},
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_DoubleAssertOfTypesWithAlignment2_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(Tuple<u16, u16>{24u, 42u}), {}, STF::AssertMetaData{42, 0, 0}, 2}}, {}, 0, 1, uint3(1,1,1)},
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoDoubleAssertOfTypesWithAlignment2_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(Tuple<u16, u16>{24u, 42u}), {}, STF::AssertMetaData{42, 0, 0}, 2}, STF::FailedAssert{serialize(Tuple<u16, u16>{1024u, 4u}), {}, STF::AssertMetaData{42, 0, 0}, 2}}, {}, 0, 2, uint3(1,1,1)},
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_DoubleAssertOfTypesWithAlignment8_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(Tuple<u64, u64>{24u, 42u}), {}, STF::AssertMetaData{42, 0, 0}, 10}}, {}, 0, 1, uint3(1,1,1)},
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoDoubleAssertOfTypesWithAlignment8_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(Tuple<u64, u64>{24u, 42u}), {}, STF::AssertMetaData{42, 0, 0}, 10}, STF::FailedAssert{serialize(Tuple<u64, u64>{1024u, 4u}), {}, STF::AssertMetaData{42, 0, 0}, 10}}, {}, 0, 2, uint3(1,1,1)},
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoDoubleAssertFirstAlign2SecondAlign8_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(Tuple<u16, u16>{24u, 42u}), {}, STF::AssertMetaData{42, 0, 0}, 2}, STF::FailedAssert{serialize(Tuple<u64, u64>{1024u, 4u}), {}, STF::AssertMetaData{42, 0, 0}, 10}}, {}, 0, 2, uint3(1,1,1)},
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoDoubleAssertFirstAlign8SecondAlign2_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(Tuple<u64, u64>{24u, 42u}), {}, STF::AssertMetaData{42, 0, 0}, 10}, STF::FailedAssert{serialize(Tuple<u16, u16>{1024u, 4u}), {}, STF::AssertMetaData{42, 0, 0}, 2}}, {}, 0, 2, uint3(1,1,1)},
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_SingleAssertOfTypesWithAlignment2_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(Tuple<u16>{24u}), {}, STF::AssertMetaData{42, 0, 0}, 2}}, {}, 0, 1, uint3(1,1,1)},
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoSingleAssertOfTypesWithAlignment2_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(u16{24u}), {}, STF::AssertMetaData{42, 0, 0}, 2}, STF::FailedAssert{serialize(u16{1024u}), {}, STF::AssertMetaData{42, 0, 0}, 2}}, {}, 0, 2, uint3(1,1,1)},
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_SingleAssertOfTypesWithAlignment8_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(u64{24u}), {}, STF::AssertMetaData{42, 0, 0}, 10}}, {}, 0, 1, uint3(1,1,1)},
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoSingleAssertOfTypesWithAlignment8_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(u64{24u}), {}, STF::AssertMetaData{42, 0, 0}, 10}, STF::FailedAssert{serialize(u64{1024u}), {}, STF::AssertMetaData{42, 0, 0}, 10}}, {}, 0, 2, uint3(1,1,1)},
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoSingleAssertFirstAlign2SecondAlign8_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(u16{24u}), {}, STF::AssertMetaData{42, 0, 0}, 2}, STF::FailedAssert{serialize(u64{1024u}), {}, STF::AssertMetaData{42, 0, 0}, 10}}, {}, 0, 2, uint3(1,1,1)},
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoSingleAssertFirstAlign8SecondAlign2_THEN_HasExpectedResults",
                    STF::TestRunResults{ {STF::FailedAssert{serialize(u64{24u}), {}, STF::AssertMetaData{42, 0, 0}, 10}, STF::FailedAssert{serialize(u16{1024u}), {}, STF::AssertMetaData{42, 0, 0}, 2}}, {}, 0, 2, uint3(1,1,1)},
                    10, 100
                }
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/AssertBuffer/ResultsProcessing/AssertInfoWithData.hlsl"), { numRecordedAsserts, numBytesData }));
    fixture.RegisterByteReader("TEST_TYPE_WITH_WRITER", [](const u16, const std::span<const std::byte>) { return ""; });

    DYNAMIC_SECTION(testName)
    {
        const auto results = fixture.RunTest(testName, 1, 1, 1);
        CAPTURE(results);
        const auto actual = results.GetTestResults();
        REQUIRE(actual);
        REQUIRE(*actual == expected);
    }
}