
#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <ranges>
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
    
    GIVEN(testName)
    {
        if (shouldSucceed)
        {
            ShaderTestFixture fixture{ ShaderTestFixture::FixtureDesc{} };

            THEN("There should be zero stats")
            {
                const auto stats = ShaderTestFixture::GetTestStats();

                REQUIRE(stats.empty());
            }

            const auto result = fixture.RunCompileTimeTest
            (
                ShaderTestFixture::CompileTestDesc
                {
                    .CompilationEnv
                    {
                        .Source = std::move(source)
                    },
                    .TestName = std::move(testName)
                }
            );

            THEN("It should succeed")
            {
                REQUIRE(result);
            }
        }
        else
        {
            ShaderTestFixture fixture{ ShaderTestFixture::FixtureDesc{} };

            THEN("There should be zero stats")
            {
                const auto stats = ShaderTestFixture::GetTestStats();

                REQUIRE(stats.empty());
            }

            const auto result = fixture.RunCompileTimeTest
            (
                ShaderTestFixture::CompileTestDesc
                {
                    .CompilationEnv
                    {
                        .Source = std::move(source)
                    },
                    .TestName = std::move(testName)
                }
            );

            THEN("It should fail")
            {
                REQUIRE_FALSE(result);
            }
        }

        THEN("Stats should be generated")
        {
            const auto stats = ShaderTestFixture::GetTestStats();
            REQUIRE(!stats.empty());

            AND_WHEN("Any future attempts to get stats")
            {
                const auto otherStats = ShaderTestFixture::GetTestStats();

                THEN("Has same number of stats")
                {
                    REQUIRE(stats.size() == otherStats.size());
                }

                THEN("Stats have the same names")
                {
                    for (const auto& [first, second] : std::views::zip(stats, otherStats))
                    {
                        REQUIRE(first.Name == second.Name);
                    }
                }
            }
        }
    }
}
