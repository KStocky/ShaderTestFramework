#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>
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
                std::tuple{"GIVEN_StringCreatorWithOver64Characters_THEN_FailsCompilation", std::vector<std::string>{"static_assert failed", "Strings with greater than 256 characters are not supported"}}
            }
        )
    );

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
                    .Source = fs::path(std::format("/Tests/String/StringLengthTests/{}.hlsl", testName))
                },
                .TestName = "Main",
                .ThreadGroupCount{1, 1, 1}
            }
        );
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