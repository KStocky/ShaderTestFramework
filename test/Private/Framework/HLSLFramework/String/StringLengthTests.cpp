#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using Catch::Matchers::ContainsSubstring;

SCENARIO("HLSLFrameworkTests - String - StrLen")
{
    auto [testName, expectedFailStrings] = GENERATE
    (
        table<std::string, std::vector<std::string>>
        (
            {
                std::tuple{"GIVEN_EmptyString_THEN_ReturnsOne", std::vector<std::string>{}},
                std::tuple{"GIVEN_NonEmptyString_THEN_ReturnsLengthOfString", std::vector<std::string>{}},
                std::tuple{"GIVEN_StringWithOver64Characters_THEN_FailsCompilation", std::vector<std::string>{"Strings with greater than 64 characters are not supported"}}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path(std::format("/Tests/String/StringLengthTests/{}.hlsl", testName))));
    DYNAMIC_SECTION(testName)
    {
        if (expectedFailStrings.empty())
        {
            REQUIRE(fixture.RunTest("Main", 1, 1, 1));
        }
        else
        {
            const auto results = fixture.RunTest(testName, 1, 1, 1);
            CAPTURE(results);
            const auto actual = results.GetFailedCompilationResult();
            REQUIRE(actual);

            std::stringstream stream;
            stream << *actual;

            for (const auto& expectedString : expectedFailStrings)
            {
                REQUIRE_THAT(stream.str(), ContainsSubstring(expectedString, Catch::CaseSensitive::No));
            }
        }
    }
}