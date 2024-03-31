#include "Framework/TestDataBufferLayout.h"
#include <array>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>


namespace TestDataSectionTests
{
    template<u32 InAlignment, u32 InSize>
    struct alignas(InAlignment) TestStruct
    {
        std::array<std::byte, InSize> NotUsed;
    };
}

TEMPLATE_TEST_CASE_SIG("TestDataSectionTests", "[template][nttp]", ((u32 InAlignment, u32 InSize), InAlignment, InSize),
    (1, 2), (1, 4), (1, 8), (1, 16),
    (2, 2), (2, 4), (2, 8), (2, 16),
    (4, 4), (4, 8), (4, 16), (4, 32),
    (8, 8), (8, 16), (8, 32), (8, 64)
)
{
    using TestDataSectionTests::TestStruct;
    using STF::TestDataSection;

    GIVEN("Meta data has Alignment: " << InAlignment << " and Size: " << InSize)
    {
        static constexpr u32 alignment = InAlignment;
        static constexpr u32 size = InSize;
        using Type = TestDataSection<TestStruct<alignment, size>>;

        const auto [actualBegin, expectedBegin] = GENERATE
        (
            table<u32, u32>
            (
                {
                    std::tuple{ 0u, 0u },
                    std::tuple{ 4u, 8u },
                    std::tuple{ 8u, 8u },
                    std::tuple{ 12u, 16u }
                }
            )
        );

        WHEN("Section created with Begin: " << actualBegin)
        {
            const auto num = GENERATE(0u, 1u, 2u, 4u, 4u);

            AND_WHEN("Num of Entries: " << num)
            {
                auto [actualSizeData, expectedSizeData] = GENERATE
                (
                    table<u32, u32>
                    (
                        {
                            std::tuple{ 0u, 0u },
                            std::tuple{ 4u, 8u },
                            std::tuple{ 8u, 8u },
                            std::tuple{ 12u, 16u }
                        }
                    )
                );

                AND_WHEN("Size of Data: " << actualSizeData)
                {
                    const Type test{ actualBegin, num, actualSizeData };

                    THEN("Values are all as expected")
                    {
                        const u32 expectedSizeMeta = static_cast<u32>(AlignedOffset(size * num, 8u));

                        REQUIRE(test.Begin() == expectedBegin);
                        REQUIRE(test.NumMeta() == num);
                        REQUIRE(test.SizeInBytesOfData() == expectedSizeData);
                        REQUIRE(test.SizeInBytesOfSection() == expectedSizeData + expectedSizeMeta);
                        REQUIRE(test.SizeInBytesOfMeta() == expectedSizeMeta);
                        REQUIRE(test.BeginData() == expectedBegin + expectedSizeMeta);
                        REQUIRE(test.EndSection() == expectedBegin + expectedSizeMeta + expectedSizeData);
                    }
                }
            }
        }
    }
}

SCENARIO("TestDataBufferLayoutTests")
{
    const auto numFailedAsserts = GENERATE(0u, 1u, 2u, 4u, 4u);

    WHEN("Num Failed Asserts: " << numFailedAsserts)
    {
        const auto [actualSizeAssertData, expectedSizeAssertData] = GENERATE
        (
            table<u32, u32>
            (
                {
                    std::tuple{ 0u, 0u },
                    std::tuple{ 4u, 8u },
                    std::tuple{ 8u, 8u },
                    std::tuple{ 12u, 16u }
                }
            )
        );

        AND_WHEN("Size of Assert Data: " << actualSizeAssertData)
        {
            const auto numStrings = GENERATE(0u, 1u, 2u, 4u, 4u);

            AND_WHEN("Num Strings: " << numStrings)
            {
                const auto [actualSizeStringData, expectedSizeStringData] = GENERATE
                (
                    table<u32, u32>
                    (
                        {
                            std::tuple{ 0u, 0u },
                            std::tuple{ 4u, 8u },
                            std::tuple{ 8u, 8u },
                            std::tuple{ 12u, 16u }
                        }
                    )
                );

                AND_WHEN("Size of String Data: " << actualSizeStringData)
                {
                    const auto numSections = GENERATE(0u, 8u, 32u, 64u);

                    AND_WHEN("Has " << numSections << " sections")
                    {
                        const STF::TestDataBufferLayout test{ numFailedAsserts, actualSizeAssertData, numStrings, actualSizeStringData, numSections };
                        THEN("Values are all as expected")
                        {
                            const auto assertSection = test.GetAssertSection();
                            const auto stringSection = test.GetStringSection();
                            const auto sectionInfoSection = test.GetSectionInfoSection();

                            REQUIRE(test.GetSizeOfTestData() == assertSection.SizeInBytesOfSection() + stringSection.SizeInBytesOfSection() + sectionInfoSection.SizeInBytesOfSection());
                            REQUIRE(assertSection.NumMeta() == numFailedAsserts);
                            REQUIRE(assertSection.SizeInBytesOfData() == expectedSizeAssertData);
                            REQUIRE(stringSection.NumMeta() == numStrings);
                            REQUIRE(stringSection.SizeInBytesOfData() == expectedSizeStringData);
                            REQUIRE(sectionInfoSection.NumMeta() == numSections);
                            REQUIRE(sectionInfoSection.SizeInBytesOfData() == 0u);
                        }
                    }
                }
            }
        }
    }
}
