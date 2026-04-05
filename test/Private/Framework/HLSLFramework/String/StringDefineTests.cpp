#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>
#include <Utility/EnumReflection.h>

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using Catch::Matchers::ContainsSubstring;

TEST_CASE_PERSISTENT_FIXTURE(ShaderTestFixtureBaseFixture, "HLSLFrameworkTests - String - Defines")
{
    using namespace stf;
    const auto stringMode = GENERATE
    (
        EStringMode::Off,
        EStringMode::On
    );

    const auto maxStringLength = static_cast<u32>(GENERATE
    (
        EStringMaxLength::s16,
        EStringMaxLength::s64,
        EStringMaxLength::s256
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
        (stringMode == EStringMode::Off) || 
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
        }();

    const auto expectedString =
        [maxStringLength, actualStringLength]()
        {
            std::stringstream buff;
            buff << "\"";

            const auto lengthToUse = std::min(actualStringLength, maxStringLength);
            for (u32 iter = 1; iter < lengthToUse; ++iter)
            {
                buff << "a";
            }

            buff << "\"";
            return buff.str();
        }();

    GIVEN("StringMode: " << Enum::UnscopedName(stringMode))
    {
        AND_GIVEN("Max String Length: " << maxStringLength)
        {
            WHEN("String is " << actualStringLength << " characters")
            {
                const auto results = fixture.RunTest(
                    ShaderTestFixture::RuntimeTestDesc
                    {
                        .CompilationEnv
                        {
                            .Source = fs::path("/Tests/String/StringDefineTests.hlsl"),
                            .Defines
                            {
                                ShaderMacro{"TEST_STRING", testString},
                                ShaderMacro{"EXPECTED_STRING", expectedString}
                            }
                        },
                        .TestName = "Test",
                        .ThreadGroupCount = {1, 1, 1},
                        .StringMaxLength = static_cast<EStringMaxLength>(maxStringLength),
                        .StringMode = stringMode
                    }
                );

                if (shouldSucceed)
                {
                    THEN("The actual string is of the expected length")
                    {
                        REQUIRE(results);
                    }
                }
                else
                {
                    THEN("The actual string should be a different length to the expected string")
                    {
                        CAPTURE(results);
                        const auto actual = results.GetTestRunError();
                        REQUIRE_FALSE(actual);
                    }
                }
            }
        }
    }
}