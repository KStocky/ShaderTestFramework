#include "Stats/StatSystem.h"
#include "Utility/Concepts.h"
#include <catch2/catch_test_macros.hpp>

namespace ScopedCPUDurationStatCompileTests
{
    using namespace stf;
    static StatSystem system;

    constexpr auto systemGetter =
        []() -> StatSystem&
        {
            return system;
        };

    using TestScopedStat = ScopedCPUDurationStat<systemGetter>;

    static_assert(!DefaultConstructibleType<TestScopedStat>);

    static_assert(!std::copyable<TestScopedStat>);
    static_assert(!std::movable<TestScopedStat>);

    static_assert(std::constructible_from<TestScopedStat, std::string>);

    static_assert(!Newable<TestScopedStat, std::string>);
}

SCENARIO("StatSystemTests")
{
    using namespace stf;
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
                    REQUIRE(stats[0].Stat.Count<std::chrono::nanoseconds>() > 0);
                }

                AND_WHEN("Stats flushed again without another scoped stat")
                {
                    auto newStats = system.FlushTimedStats();

                    THEN("Zero stats returned")
                    {
                        REQUIRE(newStats.empty());
                    }
                }

                AND_WHEN("Stats flushed again with another scoped stat")
                {
                    const std::string expectedSecondStat = "Test2";
                    {
                        TestScopedStat stat(expectedSecondStat);
                    }
                    auto newStats = system.FlushTimedStats();

                    THEN("One stat returned")
                    {
                        REQUIRE(newStats.size() == 1ull);
                    }

                    THEN("Flushed stat has expected info")
                    {
                        REQUIRE(newStats[0].Name == expectedSecondStat);
                        REQUIRE(newStats[0].Stat.Count<std::chrono::nanoseconds>() > 0);
                    }
                }
            }

            AND_WHEN("Another stat enters and leaves scope")
            {
                const std::string expectedSecondStat = "Test2";
                {
                    TestScopedStat stat(expectedSecondStat);
                }
                auto newStats = system.FlushTimedStats();

                THEN("Two stats returned")
                {
                    REQUIRE(newStats.size() == 2ull);
                }

                THEN("Flushed stats have expected info")
                {
                    REQUIRE(newStats[0].Name == expectedName);
                    REQUIRE(newStats[0].Stat.Count<std::chrono::nanoseconds>() > 0);
                    REQUIRE(newStats[1].Name == expectedSecondStat);
                    REQUIRE(newStats[1].Stat.Count<std::chrono::nanoseconds>() > 0);
                }
            }
        }

        WHEN("Stats are flushed before stats scope has ended")
        {
            {
                TestScopedStat stat("Uncompleted");

                auto stats = system.FlushTimedStats();

                THEN("There should be zero stats returned")
                {
                    REQUIRE(stats.empty());
                }

            }
            
            AND_WHEN("Stat scope ends")
            {
                THEN("The stat is has been ignored")
                {
                    auto stats = system.FlushTimedStats();
                    REQUIRE(stats.empty());
                }
            }
        }
    }
}