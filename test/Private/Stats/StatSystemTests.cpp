#include "Stats/StatSystem.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("StatSystemTests")
{
    GIVEN("A global stat system")
    {
        static StatSystem system;

        constexpr auto systemGetter =
            []() -> StatSystem&
            {
                return system;
            };

        using TestScopedStat = ScopedCPUDurationStat<systemGetter>;

        WHEN("Scoped stat enters and leaves scope")
        {
            const std::string expectedName{ "Test" };
            {
                TestScopedStat stat(expectedName);
            }

            AND_WHEN("Stats flushed")
            {
                auto stats = system.FlushTimedStats();

                THEN("Flushed stats have a single timed stat")
                {
                    REQUIRE(stats.size() == 1ull);
                }

                THEN("Flushed stat has expected info")
                {
                    REQUIRE(stats[0].Name == expectedName);
                    REQUIRE(stats[0].Stat.Count<std::chrono::microseconds>() > 0);
                }

                AND_WHEN("Stats flushed again")
                {
                    auto newStats = system.FlushTimedStats();

                    THEN("Zero stats returned")
                    {
                        REQUIRE(newStats.empty());
                    }
                }
            }
        }

        WHEN("Scoped stat is constructed")
        {
            TestScopedStat stat("Uncompleted");

            AND_WHEN("Stats are flushed")
            {
                auto stats = system.FlushTimedStats();

                THEN("There should be zero stats returned")
                {
                    REQUIRE(stats.empty());
                }
            }
        }
    }
}