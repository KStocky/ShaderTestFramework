

#include "Framework/TestDataBufferProcessor.h"

#include <Utility/Math.h>
#include <Utility/Tuple.h>
#include <Utility/TypeTraits.h>

#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace
{
    std::vector<std::byte> EncodeStringData(const STF::TestDataSection<STF::StringMetaData> InStringLayout, const std::span<std::string> InStrings)
    {
        std::vector<std::byte> ret;

        ret.resize(InStringLayout.Begin() + InStringLayout.SizeInBytesOfSection());
        std::memset(ret.data(), 0, ret.size());

        const u32 numStrings = std::min(static_cast<u32>(InStrings.size()), InStringLayout.NumMeta());

        u32 stringDataAllocated = 0;
        for (u32 stringMetaIndex = 0; stringMetaIndex < numStrings; ++stringMetaIndex)
        {
            const auto& str = InStrings[stringMetaIndex];
            const u32 stringLen = static_cast<u32>(AlignedOffset(str.size(), 4));

            if (stringLen + stringDataAllocated >= InStringLayout.SizeInBytesOfData())
            {
                break;
            }

            STF::StringMetaData meta
            {
                .DynamicDataInfo
                {
                    .DataAddress = InStringLayout.BeginData() + stringDataAllocated,
                    .DataSize = stringLen
                }
            };

            std::memcpy(ret.data() + InStringLayout.Begin() + stringMetaIndex * sizeof(STF::StringMetaData), &meta, sizeof(sizeof(STF::StringMetaData)));

            stringDataAllocated += stringLen;

            const u64 numPackedUints = AlignedOffset(str.size(), 4) / 4;
            const bool fullyPacked = numPackedUints * 4 == str.size();
            for (u64 packedIndex = 0; packedIndex < numPackedUints; ++packedIndex)
            {
                u32 packed = 0;
                if ((packedIndex != numPackedUints - 1) || fullyPacked)
                {
                    packed = packed | (str[packedIndex * 4 + 0] << 0);
                    packed = packed | (str[packedIndex * 4 + 1] << 8);
                    packed = packed | (str[packedIndex * 4 + 2] << 16);
                    packed = packed | (str[packedIndex * 4 + 3] << 24);
                }
                else
                {
                    for (u32 charIndex = 0; charIndex < (str.size() % 4); ++charIndex)
                    {
                        packed = packed | (str[packedIndex * 4 + charIndex] << (charIndex * 8));
                    }
                }

                std::memcpy(ret.data() + meta.DynamicDataInfo.DataAddress + packedIndex * sizeof(u32), &packed, sizeof(u32));
            }
        }

        return ret;
    }

    std::vector<std::byte> EncodeSectionData(const STF::TestDataSection<STF::SectionInfoMetaData> InSectionLayout, const std::span<STF::SectionInfoMetaData> InSections)
    {
        std::vector<std::byte> ret;

        ret.resize(InSectionLayout.Begin() + InSectionLayout.SizeInBytesOfSection());
        std::memset(ret.data(), 0, ret.size());

        const u32 numSections = std::min(static_cast<u32>(InSections.size()), InSectionLayout.NumMeta());

        for (u32 sectionMetaIndex = 0; sectionMetaIndex < numSections; ++sectionMetaIndex)
        {
            std::memcpy(ret.data() + InSectionLayout.Begin() + sectionMetaIndex * sizeof(STF::SectionInfoMetaData), &InSections[sectionMetaIndex], sizeof(STF::SectionInfoMetaData));
        }

        return ret;
    }

    u32 CalculateNumBytes(const std::span<std::string> InStrings)
    {
        return std::reduce(InStrings.cbegin(), InStrings.cend(), 0u,
            [](const u32 InVal, const std::string& InStr)
            {
                return InVal + static_cast<u32>(InStr.size());
            });
    }

    u32 NumStringsFitInBuffer(const std::span<std::string> InStrings, const u32 InBufferSize)
    {
        u32 running = 0;
        const u32 numStrings = static_cast<u32>(InStrings.size());
        for (u64 index = 0; index < numStrings; ++index)
        {
            const u32 size = static_cast<u32>(AlignedOffset(InStrings[index].size(), 4));
            if (running + size >= InBufferSize)
            {
                return static_cast<u32>(index);
            }
            running += size;
        }

        return numStrings;
    }

    template<typename T>
    void EncodeAssertData(const T& InSingleData, std::vector<std::byte>& InOutBytes)
    {
        static constexpr u32 size = sizeof(T);
        static constexpr u32 align = alignof(T);
        static constexpr u32 sizeAndAlign = (size << 16) | align;
        static constexpr u64 sizeOfAllocation = AlignedOffset(size + 4, 8);

        const auto oldSize = InOutBytes.size();

        InOutBytes.resize(InOutBytes.size() + sizeOfAllocation);
        auto address = InOutBytes.data() + oldSize;
        std::memcpy(address, &sizeAndAlign, sizeof(u32));
        address += AlignedOffset(sizeof(u32), align);
        std::memcpy(address, &InSingleData, size);
    }

    template<typename T>
    void EncodeAssertData(const Tuple<T,T>& InPairData, std::vector<std::byte>& InOutBytes)
    {
        static constexpr u32 size = sizeof(T);
        static constexpr u32 align = alignof(T);
        static constexpr u32 sizeAndAlign = (size << 16) | align;
        static constexpr u64 sizeSingle = AlignedOffset(size + 4, align);
        static constexpr u64 sizeOfAllocation = AlignedOffset(sizeSingle * 2, 8);

        const auto oldSize = InOutBytes.size();

        InOutBytes.resize(InOutBytes.size() + sizeOfAllocation);
        auto address = InOutBytes.data() + oldSize;
        std::memcpy(address, &sizeAndAlign, sizeof(u32));
        address += AlignedOffset(sizeof(u32), align);
        std::memcpy(address, &Get<0>(InPairData), size);
        address += AlignedOffset(size, 4);
        std::memcpy(address, &sizeAndAlign, sizeof(u32));
        address += AlignedOffset(sizeof(u32), align);
        std::memcpy(address, &Get<1>(InPairData), size);
    }

    template<typename T>
    void EncodeAssertData(const std::span<T> InAssertData, std::vector<std::byte>& InOutBytes)
    {
        for (const auto& data : InAssertData)
        {
            EncodeAssertData(data, InOutBytes);
        }
    }

    template<typename T>
    std::vector<std::byte> EncodeAssertData(const std::span<T> InAssertData)
    {
        std::vector<std::byte> ret;
        for (const auto& data : InAssertData)
        {
            EncodeAssertData(data, ret);
        }

        return ret;
    }
}

SCENARIO("TestDataBufferProcessorTests - Results - Operator Bool")
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
                std::tuple{ "Non-zero failed test run", STF::Results{ STF::TestRunResults{ {}, {}, {}, 0, 1, uint3{} } }, false }
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

SCENARIO("TestDataBufferProcessorTests - ProcessStrings")
{
    using Catch::Matchers::ContainsSubstring;

    auto [given, stringSection] = GENERATE
    (
        table<std::string, STF::TestDataSection<STF::StringMetaData>>
        (
            {
                std::tuple{ "Empty string section", STF::TestDataSection<STF::StringMetaData>{0, 0, 0} },
                std::tuple{ "Begin: 0, Num: 1, Size: 0", STF::TestDataSection<STF::StringMetaData>{0, 1, 0} },
                std::tuple{ "Begin: 0, Num: 1, Size: 15", STF::TestDataSection<STF::StringMetaData>{0, 1, 16} },
                std::tuple{ "Begin: 0, Num: 2, Size: 15", STF::TestDataSection<STF::StringMetaData>{0, 2, 16} },
                std::tuple{ "Begin: 0, Num: 1, Size: 64", STF::TestDataSection<STF::StringMetaData>{0, 1, 64} },
                std::tuple{ "Begin: 0, Num: 10, Size: 1024", STF::TestDataSection<STF::StringMetaData>{0, 10, 1024} },
                std::tuple{ "Begin: 32, Num: 1, Size: 0", STF::TestDataSection<STF::StringMetaData>{32, 1, 0} },
                std::tuple{ "Begin: 32, Num: 1, Size: 15", STF::TestDataSection<STF::StringMetaData>{32, 1, 16} },
                std::tuple{ "Begin: 32, Num: 2, Size: 15", STF::TestDataSection<STF::StringMetaData>{32, 2, 16} },
                std::tuple{ "Begin: 32, Num: 1, Size: 64", STF::TestDataSection<STF::StringMetaData>{32, 1, 64} },
                std::tuple{ "Begin: 32, Num: 10, Size: 1024", STF::TestDataSection<STF::StringMetaData>{32, 10, 1024} }
            }
        )
    );

    GIVEN(given)
    {

        auto [when, strings] = GENERATE
        (
            table<std::string, std::vector<std::string>>
            (
                {
                    std::tuple{"no strings", std::vector<std::string>{}},
                    std::tuple{"A single string", std::vector<std::string>{"Hello there!"}},
                    std::tuple{"Two small strings", std::vector<std::string>{"Hello", "There"}},
                    std::tuple{"Two medium strings", std::vector<std::string>{"Hello!!!!", "There!!!!!"}},
                    std::tuple{"One Large String", std::vector<std::string>{"Hello there, this is a very large string"}},
                    std::tuple{"Two Large Strings", std::vector<std::string>{"Hello there, this is a very large string", "Another fairly large but legal string"}}
                }
            )
        );

        WHEN(when)
        {
            auto result = STF::ProcessStrings(stringSection, static_cast<u32>(strings.size()), EncodeStringData(stringSection, strings));

            THEN("Result has expected number of strings")
            {
                const u64 numStrings = NumStringsFitInBuffer(strings, stringSection.SizeInBytesOfData());
                const u64 expectedSize = std::min(numStrings, u64{ stringSection.NumMeta() });

                REQUIRE(result.size() == expectedSize);

                if (expectedSize > 0)
                {
                    AND_THEN("Result contains expected strings")
                    {
                        for (u64 stringIndex = 0; stringIndex < expectedSize; ++stringIndex)
                        {
                            REQUIRE_THAT(strings[stringIndex], ContainsSubstring(result[stringIndex], Catch::CaseSensitive::No));
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("TestDataBufferProcessorTests - ProcessSections")
{
    auto [given, sectionInfo] = GENERATE
    (
        table<std::string, STF::TestDataSection<STF::SectionInfoMetaData>>
        (
            {
                std::tuple{ "Empty section info section", STF::TestDataSection<STF::SectionInfoMetaData>{0u, 0u, 0u} },
                std::tuple{ "Begin: 0, Num: 1", STF::TestDataSection<STF::SectionInfoMetaData>{0u, 1u, 0u} },
                std::tuple{ "Begin: 0, Num: 2", STF::TestDataSection<STF::SectionInfoMetaData>{0u, 2u, 0u} },
                std::tuple{ "Begin: 0, Num: 32", STF::TestDataSection<STF::SectionInfoMetaData>{0u, 32u, 0u} },
                std::tuple{ "Begin: 32, Num: 1", STF::TestDataSection<STF::SectionInfoMetaData>{32u, 1u, 0u} },
                std::tuple{ "Begin: 32, Num: 2", STF::TestDataSection<STF::SectionInfoMetaData>{32u, 2u, 0u} },
                std::tuple{ "Begin: 32, Num: 32", STF::TestDataSection<STF::SectionInfoMetaData>{32u, 32u, 0u} },
            }
        )
    );

    GIVEN(given)
    {

        auto [when, sections] = GENERATE
        (
            table<std::string, std::vector<STF::SectionInfoMetaData>>
            (
                {
                    std::tuple{"no sections", std::vector<STF::SectionInfoMetaData>{}},
                    std::tuple{"a single section", std::vector<STF::SectionInfoMetaData>{{0u, -1,-1}}},
                    std::tuple{"Two sections", std::vector<STF::SectionInfoMetaData>{{0u, -1, -1}, {1u, -1, -1}} },
                    std::tuple{"32 sections", std::vector<STF::SectionInfoMetaData>(32, STF::SectionInfoMetaData{0u, -1, -1})}
                }
            )
        );

        WHEN(when)
        {
            auto result = STF::ProcessSectionInfo(sectionInfo, static_cast<u32>(sections.size()), EncodeSectionData(sectionInfo, sections));

            THEN("Result has expected number of sections")
            {
                const u64 expectedSize = std::min(sections.size(), u64{sectionInfo.NumMeta()});

                REQUIRE(result.size() == expectedSize);

                if (expectedSize > 0)
                {
                    AND_THEN("Result contains expected sections")
                    {
                        for (u64 sectionIndex = 0; sectionIndex < expectedSize; ++sectionIndex)
                        {
                            REQUIRE(sections[sectionIndex].ParentId == result[sectionIndex].ParentId);
                            REQUIRE(sections[sectionIndex].SectionId == result[sectionIndex].SectionId);
                            REQUIRE(sections[sectionIndex].StringId == result[sectionIndex].StringId);
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("TestDataBufferProcessorTests - No Assert Buffer")
{
    auto [given, allocationBufferData] = GENERATE
    (
        table<std::string, STF::AllocationBufferData>
        (
            {
                std::tuple{ "Zero success and zero failing asserts", STF::AllocationBufferData{0, 0} },
                std::tuple{ "Non-Zero success and zero failing asserts", STF::AllocationBufferData{1, 0} },
                std::tuple{ "Non-Zero success and non-zero failing asserts", STF::AllocationBufferData{1, 1} },
                std::tuple{ "Zero success and non-zero failing asserts", STF::AllocationBufferData{0, 1} }
            }
        )
    );

    GIVEN(given)
    {
        WHEN("Processed")
        {
            const auto results = STF::ProcessTestDataBuffer(allocationBufferData, uint3(1, 1, 1), STF::TestDataBufferLayout{ 0,0 }, std::vector<std::byte>{}, STF::MultiTypeByteReaderMap{});

            THEN("Results has expected errors")
            {
                REQUIRE(results.NumSucceeded == allocationBufferData.NumPassedAsserts);
                REQUIRE(results.NumFailed == allocationBufferData.NumFailedAsserts);
            }
        }
    }
}

SCENARIO("TestDataBufferProcessorTests - AssertInfo - No AssertData")
{

    auto [given, numFailed, expectedNumAsserts, layout, buffer] = GENERATE
    (
        table<std::string, u32, u64, STF::TestDataBufferLayout, std::vector<STF::HLSLAssertMetaData>>
        (
            {
                std::tuple{ "One failing assert with enough capacity", 1, 1, STF::TestDataBufferLayout{1, 0}, std::vector<STF::HLSLAssertMetaData>{ STF::HLSLAssertMetaData{ 42, 0, 1 } } },
                std::tuple{ "More failing asserts than layout limit", 2, 1, STF::TestDataBufferLayout{1, 0}, std::vector<STF::HLSLAssertMetaData>{ STF::HLSLAssertMetaData{ 42, 0, 1 } } },
                std::tuple{ "Fewer failing asserts than layout limit", 1, 1, STF::TestDataBufferLayout{2, 0}, std::vector<STF::HLSLAssertMetaData>{ STF::HLSLAssertMetaData{ 42, 0, 1 }, STF::HLSLAssertMetaData{ 82, 0, 1 } } }
            }
        )
    );

    GIVEN(given)
    {
        WHEN("Processed")
        {
            const auto results = STF::ProcessTestDataBuffer(STF::AllocationBufferData{ 0, numFailed }, uint3(1, 1, 1), layout, std::as_bytes(std::span{ buffer }), STF::MultiTypeByteReaderMap{});

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

SCENARIO("TestDataBufferProcessorTests - AssertInfo - AssertData")
{
    auto [given, numFailed, expectedNumAsserts, layout, metaData, expectedAssertData] = GENERATE
    (
        table<std::string, u32, u64, STF::TestDataBufferLayout, std::vector<STF::HLSLAssertMetaData>, std::vector<u32>>
        (
            {
                std::tuple
                {
                    "One failing assert with data",
                    1, 1, STF::TestDataBufferLayout{1, 100},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 1, 8 } },
                    std::vector{4u}
                },
                std::tuple
                {
                    "One failing assert with no data",
                    1, 1, STF::TestDataBufferLayout{1, 100},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 0, 0 } },
                    std::vector<u32>{}
                },
                std::tuple
                {
                    "Two failing assert with no data",
                    2, 2, STF::TestDataBufferLayout{2, 100},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 0, 0 }, STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 0, 0 } },
                    std::vector<u32>{}
                },
                std::tuple
                {
                    "First fail has data, Second does not",
                    2, 2, STF::TestDataBufferLayout{2, 100},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 2, 8 }, STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 0, 0 } },
                    std::vector{16u}
                },
                std::tuple
                {
                    "First fail does not have data, Second does",
                    2, 2, STF::TestDataBufferLayout{2, 100},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 0, 0 }, STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 2, 8 } },
                    std::vector{16u}
                },
                std::tuple
                {
                    "Both asserts have data",
                    2, 2, STF::TestDataBufferLayout{2, 100},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 2, 8 }, STF::HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 2 + 8, 8 } },
                    std::vector{16u, 32u}
                }
            }
        )
    );

    std::vector<std::byte> buffer;
    auto assertData = EncodeAssertData(std::span{ expectedAssertData });
    const u64 sizeMetaData = metaData.size() * sizeof(STF::HLSLAssertMetaData);
    buffer.resize(sizeMetaData + assertData.size());

    std::memcpy(buffer.data(), metaData.data(), sizeMetaData);
    std::memcpy(buffer.data() + sizeMetaData, assertData.data(), assertData.size());

    GIVEN(given)
    {
        WHEN("Processed")
        {
            const auto results = STF::ProcessTestDataBuffer(STF::AllocationBufferData{ 0, numFailed }, uint3(1, 1, 1), layout, std::as_bytes(std::span{ buffer }), STF::MultiTypeByteReaderMap{});

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
                            if (metaInfo.DynamicDataInfo.DataSize > 0)
                            {
                                std::vector<std::byte> expectedData;
                                expectedData.resize(8);
                                std::memcpy(expectedData.data(), assertData.data() + (metaInfo.DynamicDataInfo.DataAddress - sizeMetaData), 8);

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

SCENARIO("TestDataBufferProcessorTests - AssertInfo - Byte Reader")
{
    using Catch::Matchers::ContainsSubstring;

    auto conv1 =
        [](const u16 InTypeId, const std::span<const std::byte> InBytes)
        {
            u32 val;
            std::memcpy(&val, InBytes.data(), sizeof(u32));
            return std::format("Reader 1: Type {}: {}", InTypeId, val);
        };

    auto conv2 =
        [](const u16 InTypeId, const std::span<const std::byte> InBytes)
        {
            u32 val;
            std::memcpy(&val, InBytes.data(), sizeof(u32));
            return std::format("Reader 2: Type {}: {}", InTypeId, val);
        };

    auto [given, numFailed, layout, metaData, expectedAssertData, expectedSubstrings] = GENERATE
    (
        table<std::string, u32, STF::TestDataBufferLayout, std::vector<STF::HLSLAssertMetaData>, std::vector<u32>, std::vector<std::string>>
        (
            {
                std::tuple
                {
                    "One failing assert with no byte reader",
                    1, STF::TestDataBufferLayout{1, 100},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 0, 0, 0, 2, sizeof(STF::HLSLAssertMetaData) * 1, 8}},
                    std::vector{42u},
                    std::vector<std::string>{"Bytes", "0x2A"}
                },
                std::tuple
                {
                    "One failing assert with byte reader",
                    1, STF::TestDataBufferLayout{1, 100},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 0, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 1, 8 } },
                    std::vector{412u},
                    std::vector<std::string>{"Reader 1", "412"}
                },
                std::tuple
                {
                    "One failing assert with different byte reader",
                    1, STF::TestDataBufferLayout{1, 100},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 0, 0, 0, 1, sizeof(STF::HLSLAssertMetaData) * 1, 8 } },
                    std::vector{412u},
                    std::vector<std::string>{"Reader 2", "412"}
                },
                std::tuple
                {
                    "Two failing asserts with same byte reader",
                    2, STF::TestDataBufferLayout{2, 100},
                    std::vector
                    {
                        STF::HLSLAssertMetaData{ 42, 0, 0, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 2, 8 },
                        STF::HLSLAssertMetaData{ 42, 0, 0, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 2 + 8, 8 }
                    },
                    std::vector{412u, 214u},
                    std::vector<std::string>{"Reader 1", "412", "214"}
                },
                std::tuple
                {
                    "Two failing asserts with different same byte reader",
                    2, STF::TestDataBufferLayout{2, 100},
                    std::vector
                    {
                        STF::HLSLAssertMetaData{ 42, 0, 0, 0, 0, 1, sizeof(STF::HLSLAssertMetaData) * 2, 8 },
                        STF::HLSLAssertMetaData{ 42, 0, 0, 0, 0, 1, sizeof(STF::HLSLAssertMetaData) * 2 + 8, 8 }
                    },
                    std::vector{412u, 214u},
                    std::vector<std::string>{"Reader 2", "412", "214"}
                },
                std::tuple
                {
                    "Two failing asserts with different byte readers",
                    2, STF::TestDataBufferLayout{2, 100},
                    std::vector
                    {
                        STF::HLSLAssertMetaData{ 42, 0, 0, 0, 0, 0, sizeof(STF::HLSLAssertMetaData) * 2, 8 },
                        STF::HLSLAssertMetaData{ 42, 0, 0, 0, 0, 1, sizeof(STF::HLSLAssertMetaData) * 2 + 8, 8 }
                    },
                    std::vector{412u, 214u},
                    std::vector<std::string>{"Reader 1", "Reader 2", "412", "214"}
                }
            }
        )
    );

    std::vector<std::byte> buffer;
    auto assertData = EncodeAssertData(std::span{ expectedAssertData });
    const u64 sizeMetaData = metaData.size() * sizeof(STF::HLSLAssertMetaData);
    buffer.resize(sizeMetaData + assertData.size());

    std::memcpy(buffer.data(), metaData.data(), sizeMetaData);
    std::memcpy(buffer.data() + sizeMetaData, assertData.data(), assertData.size());

    GIVEN(given)
    {
        WHEN("Processed")
        {
            const auto results = STF::ProcessTestDataBuffer(STF::AllocationBufferData{ 0, numFailed }, uint3(1, 1, 1), layout, std::as_bytes(std::span{ buffer }), STF::MultiTypeByteReaderMap{ conv1, conv2 });

            THEN("Results has expected sub strings")
            {
                std::stringstream stream;
                stream << results;

                for (const auto expectedString : expectedSubstrings)
                {
                    REQUIRE_THAT(stream.str(), ContainsSubstring(expectedString, Catch::CaseSensitive::No));
                }
            }
        }
    }
}

SCENARIO("TestDataBufferProcessorTests - AssertInfo - Throwing")
{

    auto [given, numFailed, expectedNumAsserts, layout, buffer] = GENERATE
    (
        table<std::string, u32, u64, STF::TestDataBufferLayout, std::vector<STF::HLSLAssertMetaData>>
        (
            {
                std::tuple{ "One failing assert with empty buffer", 1, 1, STF::TestDataBufferLayout{1, 0}, std::vector<STF::HLSLAssertMetaData>{} },
                std::tuple{ "Fewer failing asserts than layout limit", 3, 1, STF::TestDataBufferLayout{3, 0}, std::vector<STF::HLSLAssertMetaData>{ STF::HLSLAssertMetaData{ 42, 0, 1 }, STF::HLSLAssertMetaData{ 82, 0, 1 } } }
            }
        )
    );

    GIVEN(given)
    {
        WHEN("Processed")
        {
            THEN("throws")
            {
                REQUIRE_THROWS(STF::ProcessTestDataBuffer(STF::AllocationBufferData{ 0, numFailed }, uint3(1, 1, 1), layout, std::as_bytes(std::span{ buffer }), STF::MultiTypeByteReaderMap{}));
            }
        }
    }
}

SCENARIO("TestDataBufferProcessorTests - FailedAssert - Equality")
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
                    "Differing ByteReaders",
                    STF::FailedAssert{{}, [](const u16, const std::span<const std::byte>) { return std::string{}; }, {}},
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

SCENARIO("TestDataBufferProcessorTests - ThreadInfoToString")
{
    using Catch::Matchers::ContainsSubstring;

    auto [given, numFailed, expectedNumAsserts, dims, layout, buffer, expectedSubstrings, expectedAbsentSubstrings] = GENERATE
    (
        table<std::string, u32, u64, uint3, STF::TestDataBufferLayout, std::vector<STF::HLSLAssertMetaData>, std::vector<std::string>, std::vector<std::string>>
        (
            {
                std::tuple
                {
                    "No thread id type and zero id",
                    1, 1, uint3(10, 10, 10),
                    STF::TestDataBufferLayout{1, 0},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 0 } },
                    std::vector<std::string>{"line"},
                    std::vector<std::string>{"threadid"}
                },
                std::tuple
                {
                    "No thread id type and non zero id",
                    1, 1, uint3(10, 10, 10),
                    STF::TestDataBufferLayout{1, 0},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 34, 0 } },
                    std::vector<std::string>{"line"},
                    std::vector<std::string>{"threadid"}
                },
                std::tuple
                {
                    "int thread id type and zero id",
                    1, 1, uint3(10, 10, 10),
                    STF::TestDataBufferLayout{1, 0},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 1 } },
                    std::vector<std::string>{"0"},
                    std::vector<std::string>{}
                },
                std::tuple
                {
                    "int thread id type and non zero id",
                    1, 1, uint3(10, 10, 10),
                    STF::TestDataBufferLayout{1, 0},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 34, 1 } },
                    std::vector<std::string>{"34"},
                    std::vector<std::string>{}
                },
                std::tuple
                {
                    "int3 thread id type and zero id",
                    1, 1, uint3(10, 10, 10),
                    STF::TestDataBufferLayout{1, 0},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 0, 2 } },
                    std::vector<std::string>{"0"},
                    std::vector<std::string>{}
                },
                std::tuple
                {
                    "int3 thread id type and non zero id",
                    1, 1, uint3(10, 10, 10),
                    STF::TestDataBufferLayout{1, 0},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 34, 2 } },
                    std::vector<std::string>{"4, 3, 0"},
                    std::vector<std::string>{}
                },
                std::tuple
                {
                    "int3 thread id type and different non zero id",
                    1, 1, uint3(10, 10, 10),
                    STF::TestDataBufferLayout{1, 0},
                    std::vector{ STF::HLSLAssertMetaData{ 42, 342, 2 } },
                    std::vector<std::string>{"2, 4, 3"},
                    std::vector<std::string>{}
                }
            }
        )
    );

    GIVEN(given)
    {
        WHEN("Processed")
        {
            const auto results = STF::ProcessTestDataBuffer(STF::AllocationBufferData{ 0, numFailed }, dims, layout, std::as_bytes(std::span{ buffer }), STF::MultiTypeByteReaderMap{});

            THEN("Results has expected sub strings")
            {
                std::stringstream stream;
                stream << results;

                for (const auto& expectedString : expectedSubstrings)
                {
                    REQUIRE_THAT(stream.str(), ContainsSubstring(expectedString, Catch::CaseSensitive::No));
                }

                for (const auto& expectedString : expectedAbsentSubstrings)
                {
                    REQUIRE_THAT(stream.str(), !ContainsSubstring(expectedString, Catch::CaseSensitive::No));
                }
            }
        }
    }
}
