#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

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