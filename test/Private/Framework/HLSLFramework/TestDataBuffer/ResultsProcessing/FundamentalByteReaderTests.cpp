#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_predicate.hpp>

SCENARIO("HLSLFrameworkTests - TestDataBuffer - ResultProcessing - FundamentalByteReader")
{
    using Catch::Matchers::Predicate;

    auto [testName, expectedSubstrings, unexpectedSubstrings] = GENERATE
    (
        table<std::string, std::vector<const char*>, std::vector<const char*>>
        (
            {
                std::tuple
                {
                    "GIVEN_TwoBools_WHEN_Failed_THEN_OutputHasExpectedSubstrings",
                    std::vector{"true", "false"},
                    std::vector<const char*>{"\n", ","}
                },
                std::tuple
                {
                    "GIVEN_TwoScalarUnsignedIntegers_WHEN_Failed_THEN_OutputHasExpectedSubstrings",
                    std::vector{"32", "42"},
                    std::vector<const char*>{"\n", ","}
                },
                std::tuple
                {
                    "GIVEN_TwoScalarNegativeIntegers_WHEN_Failed_THEN_OutputHasExpectedSubstrings",
                    std::vector{"-32", "-42"},
                    std::vector<const char*>{"\n", ","}
                }
                ,
                std::tuple
                {
                    "GIVEN_TwoScalarFloatingPoint_WHEN_Failed_THEN_OutputHasExpectedSubstrings",
                    std::vector{"32.5", "42.5"},
                    std::vector<const char*>{"\n", ","}
                },
                std::tuple
                {
                    "GIVEN_TwoScalarUnsignedIntegers16bit_WHEN_Failed_THEN_OutputHasExpectedSubstrings",
                    std::vector{"32", "42"},
                    std::vector<const char*>{"\n", ","}
                },
                std::tuple
                {
                    "GIVEN_TwoScalarNegativeIntegers16bit_WHEN_Failed_THEN_OutputHasExpectedSubstrings",
                    std::vector{"-32", "-42"},
                    std::vector<const char*>{"\n", ","}
                },
                std::tuple
                {
                    "GIVEN_TwoScalarFloatingPoint16bit_WHEN_Failed_THEN_OutputHasExpectedSubstrings",
                    std::vector{"3.25", "4.25"},
                    std::vector<const char*>{"\n", ","}
                },
                std::tuple
                {
                    "GIVEN_TwoVector3FloatingPoint64bit_WHEN_Failed_THEN_OutputHasExpectedSubstrings",
                    std::vector{"32.5, 32.5, 32.5", "42.5, 42.5, 42.5"},
                    std::vector<const char*>{"\n"}
                },
                std::tuple
                {
                    "GIVEN_TwoMatrix2x2Integer64bit_WHEN_Failed_THEN_OutputHasExpectedSubstrings",
                    std::vector{"32, 32\n32, 32", "42, 42\n42, 42"},
                    std::vector<const char*>{}
                }
            }
        )
    );

    const auto byteReaderMatcher =
        [](const std::string_view In, std::string_view InStringToMatch)
        {
            const std::string_view startingSequence1 = "Data 1: ";
            const std::string_view startingSequence2 = "Data 2: ";
            const auto maybeBegin = In.find(startingSequence1);
            if (maybeBegin == std::string::npos)
            {
                return false;
            }
            const auto begin1 = maybeBegin + startingSequence1.size();

            const auto maybeEnd = In.find(startingSequence2);
            if (maybeEnd == std::string::npos)
            {
                return false;
            }

            const auto end1 = maybeEnd - 1;
            const auto begin2 = maybeEnd + startingSequence2.size();
            const auto end2 = In.size() - 1;

            const auto firstData = In.substr(begin1, end1 - begin1);
            const auto secondData = In.substr(begin2, end2 - begin2);

            return firstData.find(InStringToMatch) != std::string::npos || secondData.find(InStringToMatch) != std::string::npos;
        };

    const auto byteReaderPredicate =
        [byteReaderMatcher, &expectedSubstrings, &unexpectedSubstrings](const std::string& In)
        {
            for (const auto expectedString : expectedSubstrings)
            {
                if (!byteReaderMatcher(In, expectedString))
                {
                    return false;
                }
            }

            for (const auto unexpectedString : unexpectedSubstrings)
            {
                if (byteReaderMatcher(In, unexpectedString))
                {
                    return false;
                }
            }

            return true;
        };

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/TestDataBuffer/ResultsProcessing/FundamentalByteReader.hlsl"), { 10, 400 }));

    DYNAMIC_SECTION(testName)
    {
        const auto results = fixture.RunTest(testName, 1, 1, 1);
        CAPTURE(results);
        const auto actual = results.GetTestResults();
        REQUIRE(actual);

        std::stringstream stream;
        stream << *actual;

        REQUIRE_THAT(stream.str(), Predicate<std::string>(byteReaderPredicate));
    }
}