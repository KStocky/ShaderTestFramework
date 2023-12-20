

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
                std::tuple{ "Constructed from empty string", STF::Results{STF::FailedShaderCompilationResult{""}}, false},
                std::tuple{ "Constructed from non-empty string", STF::Results{STF::FailedShaderCompilationResult{"Hello"}}, false },
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

SCENARIO("AssertBufferProcessorTests - AssertInfo - AssertData")
{
    auto serialize = []<typename T>(const std::span<T> InVals)
    {
        std::vector<std::byte> ret;
        ret.resize(InVals.size_bytes() + InVals.size() * sizeof(u32));
        const u64 sizeData = sizeof(T);
        for (u64 writeIndex = 0; const auto& val : InVals)
        {
            std::memcpy(ret.data() + writeIndex, &sizeData, sizeof(u32));
            writeIndex += sizeof(u32);

            std::memcpy(ret.data() + writeIndex, &val, sizeData);
            writeIndex += sizeData;
        }

        return ret;
    };

    auto [given, numFailed, expectedNumAsserts, layout, metaData, expectedAssertData] = GENERATE
    (
        table<std::string, u32, u64, STF::AssertBufferLayout, std::vector<STF::HLSLAssertMetaData>, std::vector<u32>>
        (
            {
                std::tuple
                { 
                    "One failing assert with data", 
                    1, 1, STF::AssertBufferLayout{1, 100}, 
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 1, 8 } },
                    std::vector{4u}
                },
                std::tuple
                {
                    "One failing assert with no data",
                    1, 1, STF::AssertBufferLayout{1, 100},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 0, 0 } },
                    std::vector<u32>{}
                },
                std::tuple
                {
                    "Two failing assert with no data",
                    2, 2, STF::AssertBufferLayout{2, 100},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 0, 0 }, STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 0, 0 } },
                    std::vector<u32>{}
                },
                std::tuple
                {
                    "First fail has data, Second does not",
                    2, 2, STF::AssertBufferLayout{2, 100},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 2, 8 }, STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 0, 0 } },
                    std::vector{16u}
                },
                std::tuple
                {
                    "First fail does not have data, Second does",
                    2, 2, STF::AssertBufferLayout{2, 100},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 0, 0 }, STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 2, 8 } },
                    std::vector{16u}
                },
                std::tuple
                {
                    "Both asserts have data",
                    2, 2, STF::AssertBufferLayout{2, 100},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 2, 8 }, STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 2 + 8, 8 } },
                    std::vector{16u, 32u}
                }
            }
        )
    );

    std::vector<std::byte> buffer;
    auto assertData = serialize(std::span{ expectedAssertData });
    const u64 sizeMetaData = metaData.size() * sizeof(STF::HLSLAssertMetaData);
    buffer.resize(sizeMetaData + assertData.size());

    std::memcpy(buffer.data(), metaData.data(), sizeMetaData);
    std::memcpy(buffer.data() + sizeMetaData, assertData.data(), assertData.size());

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
                    AND_THEN("assert data is as expected")
                    {
                        for (u64 assertIndex = 0; assertIndex < expectedNumAsserts; ++assertIndex)
                        {
                            const auto& metaInfo = metaData[assertIndex];
                            if (metaInfo.DataSize > 0)
                            {
                                std::vector<std::byte> expectedData;
                                expectedData.resize(8);
                                std::memcpy(expectedData.data(), assertData.data() + (metaInfo.DataAddress - sizeMetaData), 8);

                                REQUIRE(results.FailedAsserts[assertIndex].Data == expectedData);
                            }
                            else
                            {
                                REQUIRE(results.FailedAsserts[assertIndex].Data.empty());
                            }
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

SCENARIO("AssertBufferProcessorTests - FailedAssert - Equality")
{

    auto [given, left, right, expectedIsEqual] = GENERATE
    (
        table<std::string, STF::FailedAssert, STF::FailedAssert, bool>
        (
            {
                std::tuple
                {
                    "Both sides are default constructed",
                    STF::FailedAssert{},
                    STF::FailedAssert{},
                    true
                },
                std::tuple
                {
                    "Differing data",
                    STF::FailedAssert{std::vector{std::byte{1}, std::byte{2}}, {}, {}},
                    STF::FailedAssert{},
                    false
                },
                std::tuple
                {
                    "Differing TypeConverter",
                    STF::FailedAssert{{}, [](const std::span<const std::byte>) { return std::string{}; }, {}},
                    STF::FailedAssert{},
                    true
                },
                std::tuple
                {
                    "Differing Meta data",
                    STF::FailedAssert{{}, {}, {1, 0, 0}},
                    STF::FailedAssert{},
                    false
                }
            }
        )
    );

    const auto then = expectedIsEqual ? std::string_view{ "is true" } : std::string_view{ "is false" };

    GIVEN(given)
    {
        WHEN("Equals called")
        {
            const auto result = left == right;
            THEN(then)
            {
                REQUIRE(result == expectedIsEqual);
            }
            
        }

        WHEN("Not equals called")
        {
            const auto result = left != right;
            THEN(then)
            {
                REQUIRE(result != expectedIsEqual);
            }
        }
    }
}