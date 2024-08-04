#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>
#include <Utility/EnumReflection.h>

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using Catch::Matchers::ContainsSubstring;

TEST_CASE_FIXTURE(ShaderTestFixtureBaseFixture, "HLSLFrameworkTests - String - Defines")
{
    const auto stringMode = GENERATE
    (
        ShaderTestFixture::EStringMode::Off,
        ShaderTestFixture::EStringMode::On
    );

    const auto maxStringLength = static_cast<u32>(GENERATE
    (
        ShaderTestFixture::EStringMaxLength::s16,
        ShaderTestFixture::EStringMaxLength::s64,
        ShaderTestFixture::EStringMaxLength::s256
    ));

    const auto actualStringLength = GENERATE
    (
        15u,
        31u,
        63u,
        127u,
        255u,
        511u
    );

    const bool shouldSucceed = 
        (stringMode == ShaderTestFixture::EStringMode::Off) || 
        (actualStringLength < maxStringLength);

    const auto testString = 
        [actualStringLength]()
        {
            std::stringstream buff;
            buff << "\"";

            for (u32 iter = 1; iter < actualStringLength; ++iter)
            {
                buff << "a";
            }

            buff << "\"";
            return buff.str();
        };

    GIVEN("StringMode: " << Enum::UnscopedName(stringMode))
    {
        AND_GIVEN("Max String Length: " << maxStringLength)
        {
            WHEN("String is " << actualStringLength << " characters")
            {
                const auto results = fixture.RunCompileTimeTest(
                    ShaderTestFixture::CompileTestDesc
                    {
                        .CompilationEnv
                        {
                            .Source = fs::path("/Tests/String/StringDefineTests.hlsl"),
                            .Defines
                            {
                                ShaderMacro{"TTL_ENABLE_STRINGS", stringMode == ShaderTestFixture::EStringMode::On ? "1" : "0"},
                                ShaderMacro{"TTL_STRING_MAX_LENGTH", std::to_string(maxStringLength)},
                                ShaderMacro{"TEST_STRING", testString()}
                            }
                        },
                        .TestName = "String Define Test"
                    }
                );

                if (shouldSucceed)
                {
                    THEN("Shader should compile")
                    {
                        REQUIRE(results);
                    }
                }
                else
                {
                    THEN("Shader should fail compilation with expected error")
                    {
                        CAPTURE(results);
                        const auto actual = results.GetFailedCompilationResult();
                        REQUIRE(actual);

                        std::stringstream stream;
                        stream << *actual;

                        const auto expectedString = std::format("Strings with greater than {} characters are not supported", maxStringLength);

                        REQUIRE_THAT(stream.str(), ContainsSubstring(expectedString, Catch::CaseSensitive::No));
                    }
                }
            }
        }
    }
}