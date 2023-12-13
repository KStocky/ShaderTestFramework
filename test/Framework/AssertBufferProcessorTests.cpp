

#include "Framework/AssertBufferProcessor.h"

#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("AssertBufferProcessorTests")
{
    auto [given, expected, numSuccess, numFailed, assertBuffer, typeMapper] = GENERATE
    (
        table<std::string, u64, u32, u32, std::vector<std::byte>, STF::TypeConverterMap>
        (
            {
                std::tuple{"Zero success and zero failing asserts", 0, 0, 0, std::vector<std::byte>{}, STF::TypeConverterMap{}},
                std::tuple{"Non-Zero success and zero failing asserts", 0, 1, 0, std::vector<std::byte>{}, STF::TypeConverterMap{}},
                std::tuple{"Non-Zero success and non-zero failing asserts", 1, 1, 1, std::vector<std::byte>{}, STF::TypeConverterMap{}},
                std::tuple{"Zero success and non-zero failing asserts", 1, 0, 1, std::vector<std::byte>{}, STF::TypeConverterMap{}}
            }
        )
    );

    GIVEN(given)
    {
        WHEN("Processed")
        {
            const auto results = STF::ProcessAssertBuffer(numSuccess, numFailed, assertBuffer, typeMapper);

            THEN("Results has expected errors")
            {
                REQUIRE(results.size() == expected);
            }
        }
    }
}