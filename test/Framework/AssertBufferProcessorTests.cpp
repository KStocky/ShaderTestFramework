

#include "Framework/AssertBufferProcessor.h"

#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("AssertBufferProcessorTests - Results")
{

    auto [given, results, expected] = GENERATE
    (
        table<std::string, STF::Results, bool>
        (
            {
                std::tuple{ "Default constructed", STF::Results{}, false },
                std::tuple{ "Constructed from empty string", STF::Results{""}, false},
                std::tuple{ "Constructed from non-empty string", STF::Results{"Hello"}, false },
                std::tuple{ "Zero Failed test run", STF::Results{ STF::TestRunResults{} }, true },
                std::tuple{ "Non-zero failed test run", STF::Results{ STF::TestRunResults{ {}, 0, 1, uint3{} } }, false }
            }
        )
    );

    GIVEN(given)
    {
        WHEN("operator bool called")
        {
            const bool actual = results;
            THEN("result is as expected")
            {
                REQUIRE(actual == expected);
            }
        }
    }
}

SCENARIO("AssertBufferProcessorTests - No Assert Buffer")
{
    auto [given, numSuccess, numFailed] = GENERATE
    (
        table<std::string, u32, u32>
        (
            {
                std::tuple{ "Zero success and zero failing asserts", 0, 0 },
                std::tuple{ "Non-Zero success and zero failing asserts", 1, 0 },
                std::tuple{ "Non-Zero success and non-zero failing asserts", 1, 1 },
                std::tuple{ "Zero success and non-zero failing asserts", 0, 1 }
            }
        )
    );

    GIVEN(given)
    {
        WHEN("Processed")
        {
            const auto results = STF::ProcessAssertBuffer(numSuccess, numFailed, uint3(1,1,1), STF::AssertBufferLayout{ 0,0 }, std::vector<std::byte>{}, STF::TypeConverterMap{});

            THEN("Results has expected errors")
            {
                REQUIRE(results.NumSucceeded == numSuccess);
                REQUIRE(results.NumFailed == numFailed);
            }
        }
    }
}

SCENARIO("AssertBufferProcessorTests - AssertInfo - No AssertData")
{

    auto [given, numFailed, expectedNumAsserts, layout, buffer] = GENERATE
    (
        table<std::string, u32, u64, STF::AssertBufferLayout, std::vector<STF::HLSLAssertMetaData>>
        (
            {
                std::tuple{ "One failing assert with enough capacity", 1, 1, STF::AssertBufferLayout{1, 0}, std::vector<STF::HLSLAssertMetaData>{ STF::HLSLAssertMetaData{ 42, 0, 1 } } },
                std::tuple{ "More failing asserts than layout limit", 2, 1, STF::AssertBufferLayout{1, 0}, std::vector<STF::HLSLAssertMetaData>{ STF::HLSLAssertMetaData{ 42, 0, 1 } } },
                std::tuple{ "Fewer failing asserts than layout limit", 1, 1, STF::AssertBufferLayout{2, 0}, std::vector<STF::HLSLAssertMetaData>{ STF::HLSLAssertMetaData{ 42, 0, 1 }, STF::HLSLAssertMetaData{ 82, 0, 1 } } }
            }
        )
    );

    GIVEN(given)
    {
        WHEN("Processed")
        {
            const auto results = STF::ProcessAssertBuffer(0, numFailed, uint3(1, 1, 1), layout, std::as_bytes(std::span{ buffer }), STF::TypeConverterMap{});

            THEN("Results has expected errors")
            {
                REQUIRE(results.NumSucceeded == 0);
                REQUIRE(results.NumFailed == numFailed);
                REQUIRE(results.FailedAsserts.size() == expectedNumAsserts);

                if (expectedNumAsserts > 0)
                {
                    AND_THEN("asserts are as expected")
                    {
                        for (u64 assertIndex = 0; assertIndex < expectedNumAsserts; ++assertIndex)
                        {
                            REQUIRE(buffer[assertIndex] == results.FailedAsserts[assertIndex].Info);
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("AssertBufferProcessorTests - AssertInfo - Throwing")
{

    auto [given, numFailed, expectedNumAsserts, layout, buffer] = GENERATE
    (
        table<std::string, u32, u64, STF::AssertBufferLayout, std::vector<STF::HLSLAssertMetaData>>
        (
            {
                std::tuple{ "One failing assert with empty buffer", 1, 1, STF::AssertBufferLayout{1, 0}, std::vector<STF::HLSLAssertMetaData>{} },
                std::tuple{ "Fewer failing asserts than layout limit", 3, 1, STF::AssertBufferLayout{3, 0}, std::vector<STF::HLSLAssertMetaData>{ STF::HLSLAssertMetaData{ 42, 0, 1 }, STF::HLSLAssertMetaData{ 82, 0, 1 } } }
            }
        )
    );

    GIVEN(given)
    {
        WHEN("Processed")
        {
            THEN("throws")
            {
                REQUIRE_THROWS(STF::ProcessAssertBuffer(0, numFailed, uint3(1, 1, 1), layout, std::as_bytes(std::span{ buffer }), STF::TypeConverterMap{}));
            }
        }
    }
}