#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

SCENARIO("HLSLFrameworkTests - AssertBuffer - ResultProcessing - ByteReader")
{
    using Catch::Matchers::ContainsSubstring;

    auto [testName, expectedSubstrings] = GENERATE
    (
        table<std::string, std::vector<const char*>>
        (
            {
                std::tuple
                {
                    "GIVEN_FailedSingleAssert_WHEN_NoReaderId_THEN_HasExpectedResults",
                    std::vector{"Undefined"}
                },
                std::tuple
                {
                    "GIVEN_FailedSingleAssert_WHEN_ReaderId_THEN_HasExpectedResults",
                    std::vector{"Reader 1", "12345678"}
                },
                std::tuple
                {
                    "GIVEN_FailedTwoSingleAsserts_WHEN_FirstNoReaderIdSecondHasReaderId_THEN_HasExpectedResults",
                    std::vector{"Reader 1", "12345678", "Undefined"}
                },
                std::tuple
                {
                    "GIVEN_FailedTwoSingleAsserts_WHEN_BothHaveSameReaderId_THEN_HasExpectedResults",
                    std::vector{"Reader 1", "12345678", "1234"}
                },
                std::tuple
                {
                    "GIVEN_FailedTwoSingleAsserts_WHEN_BothHaveDifferentReaderId_THEN_HasExpectedResults",
                    std::vector{"Reader 1", "12345678", "Reader 2", "87654321"}
                }
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/AssertBuffer/ResultsProcessing/FundamentalByteReader.hlsl"), { 10, 400 }));

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