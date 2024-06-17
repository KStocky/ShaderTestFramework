#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <ranges>
#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("BasicShaderTests")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"ThisTestDoesNotExist", false},
                std::tuple{"ThisTestShouldPass", true},
                std::tuple{"ThisTestShouldFail", false},
            }
        )
    );

    GIVEN(testName)
    {
        if (shouldSucceed)
        {
            ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/BasicShaderTests.hlsl")));

            THEN("There should be zero stats")
            {
                const auto stats = ShaderTestFixture::GetTestStats();

                REQUIRE(stats.empty());
            }

            THEN("It should succeed")
            {
                REQUIRE(fixture.RunTest(testName, 1, 1, 1));
            }
        }
        else
        {
            ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/BasicShaderTests.hlsl")));

            THEN("There should be zero stats")
            {
                const auto stats = ShaderTestFixture::GetTestStats();

                REQUIRE(stats.empty());
            }

            THEN("It should fail")
            {
                const auto result = fixture.RunTest(testName, 1, 1, 1);
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