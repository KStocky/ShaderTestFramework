
#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

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

SCENARIO("ShaderTestFixtureTests - Run Compile Time Tests")
{
    auto [testName, source, shouldSucceed] = GENERATE
    (
        table<std::string, std::string, bool>
        (
            {
                std::tuple
                {
                    "InvalidShader",
                    R"(
                        void Main()
                        {
                            asdfasdf
                        }
                    )",
                    false
                },
                std::tuple
                {
                    "ValidShader",
                    R"(
                        void Main()
                        {
                        }
                    )",
                    true
                }
            }
        )
    );
    
    ShaderTestFixture fixture(ShaderTestFixture::Desc{.Source = std::move(source)});
    GIVEN(testName)
    {
        const auto result = fixture.RunCompileTimeTest();
        if (shouldSucceed)
        {
            THEN("It should succeed")
            {
                REQUIRE(result);
            }
        }
        else
        {
            THEN("It should fail")
            {
                REQUIRE_FALSE(result);
            }
        }
    }
}
