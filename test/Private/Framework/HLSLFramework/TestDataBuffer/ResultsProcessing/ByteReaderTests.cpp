#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

class ByteReaderTestsFixture : public ShaderTestFixtureBaseFixture
{
public:
    ByteReaderTestsFixture()
        : ShaderTestFixtureBaseFixture()
    {
        fixture.RegisterByteReader("TEST_READER_1",
            [](const stf::u16, const std::span<const std::byte> InBytes)
            {
                stf::u32 value;
                std::memcpy(&value, InBytes.data(), sizeof(stf::u32));
                return std::format("Reader 1: {}", value);
            });
        fixture.RegisterByteReader("TEST_READER_2",
            [](const stf::u16, const std::span<const std::byte> InBytes)
            {
                stf::u32 value;
                std::memcpy(&value, InBytes.data(), sizeof(stf::u32));
                return std::format("Reader 2: {}", value);
            });
    }
};

TEST_CASE_PERSISTENT_FIXTURE(ByteReaderTestsFixture, "HLSLFrameworkTests - TestDataBuffer - ResultProcessing - ByteReader")
{
    using namespace stf;
    using Catch::Matchers::ContainsSubstring;

    auto [testName, expectedSubstrings] = GENERATE
    (
        table<std::string, std::vector<const char*>>
        (
            {
                std::tuple
                {
                    "GIVEN_FailedSingleAssert_WHEN_NoReaderId_THEN_HasExpectedResults",
                    std::vector{"Undefined"}
                },
                std::tuple
                {
                    "GIVEN_FailedSingleAssert_WHEN_ReaderId_THEN_HasExpectedResults",
                    std::vector{"Reader 1", "12345678"}
                },
                std::tuple
                {
                    "GIVEN_FailedTwoSingleAsserts_WHEN_FirstNoReaderIdSecondHasReaderId_THEN_HasExpectedResults",
                    std::vector{"Reader 1", "12345678", "Undefined"}
                },
                std::tuple
                {
                    "GIVEN_FailedTwoSingleAsserts_WHEN_BothHaveSameReaderId_THEN_HasExpectedResults",
                    std::vector{"Reader 1", "12345678", "1234"}
                },
                std::tuple
                {
                    "GIVEN_FailedTwoSingleAsserts_WHEN_BothHaveDifferentReaderId_THEN_HasExpectedResults",
                    std::vector{"Reader 1", "12345678", "Reader 2", "87654321"}
                }
            }
        )
    );

    DYNAMIC_SECTION(testName)
    {
        const auto results = fixture.RunTest(
            ShaderTestFixture::RuntimeTestDesc
            {
                .CompilationEnv
                {
                    .Source = fs::path("/Tests/TestDataBuffer/ResultsProcessing/ByteReader.hlsl")
                },
                .TestName = testName,
                .ThreadGroupCount{1, 1, 1},
                .TestDataLayout
                {
                    .NumFailedAsserts = 10,
                    .NumBytesAssertData = 400
                }
            }
        );
        CAPTURE(results);
        const auto actual = results.GetTestResults();
        REQUIRE(actual);

        std::stringstream stream;
        stream << *actual;

        for (const auto expectedString : expectedSubstrings)
        {
            REQUIRE_THAT(stream.str(), ContainsSubstring(expectedString, Catch::CaseSensitive::No));
        }
    }
}