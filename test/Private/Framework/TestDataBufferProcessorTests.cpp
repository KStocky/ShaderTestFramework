

#include "Framework/TestDataBufferProcessor.h"

#include <Utility/Math.h>
#include <Utility/Tuple.h>
#include <Utility/TypeTraits.h>

#include <algorithm>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

namespace TestDataBufferProcessorTests
{
    using namespace stf;
    namespace
    {
        std::vector<std::byte> EncodeStringData(const TestDataSection<StringMetaData> InStringLayout, const std::span<std::string> InStrings)
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

                StringMetaData meta
                {
                    .DynamicDataInfo
                    {
                        .DataAddress = InStringLayout.BeginData() + stringDataAllocated,
                        .DataSize = stringLen
                    }
                };

                std::memcpy(ret.data() + InStringLayout.Begin() + stringMetaIndex * sizeof(StringMetaData), &meta, sizeof(sizeof(StringMetaData)));

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

        std::vector<std::byte> EncodeSectionData(const TestDataSection<SectionInfoMetaData> InSectionLayout, const std::span<SectionInfoMetaData> InSections)
        {
            std::vector<std::byte> ret;

            ret.resize(InSectionLayout.Begin() + InSectionLayout.SizeInBytesOfSection());
            std::memset(ret.data(), 0, ret.size());

            const u32 numSections = std::min(static_cast<u32>(InSections.size()), InSectionLayout.NumMeta());

            for (u32 sectionMetaIndex = 0; sectionMetaIndex < numSections; ++sectionMetaIndex)
            {
                std::memcpy(ret.data() + InSectionLayout.Begin() + sectionMetaIndex * sizeof(SectionInfoMetaData), &InSections[sectionMetaIndex], sizeof(SectionInfoMetaData));
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
            std::memcpy(address, &get<0>(InPairData), size);
            address += AlignedOffset(size, 4);
            std::memcpy(address, &sizeAndAlign, sizeof(u32));
            address += AlignedOffset(sizeof(u32), align);
            std::memcpy(address, &get<1>(InPairData), size);
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
        std::vector<std::byte> EncodeAssertData(const std::vector<T>& InAssertData)
        {
            std::vector<std::byte> ret;
            for (const auto& data : InAssertData)
            {
                EncodeAssertData(data, ret);
            }

            return ret;
        }

        auto CreateByteReader(const std::string_view InSalt)
        {
            return 
                [InSalt](const u16 InTypeId, const std::span<const std::byte> InBytes)
                {
                    u32 val;
                    std::memcpy(&val, InBytes.data(), sizeof(u32));
                    return std::format("{}: Type {}: {}", InSalt, InTypeId, val);
                };
        }
    }

    SCENARIO("TestDataBufferProcessorTests - Results - Operator Bool")
    {

        auto [given, results, expected] = GENERATE
        (
            table<std::string, Results, bool>
            (
                {
                    std::tuple{ "Default constructed", Results{}, false },
                    std::tuple{ "Constructed from empty string", Results{ ErrorTypeAndDescription{ ETestRunErrorType::Unknown, ""} }, false},
                    std::tuple{ "Constructed from non-empty string", Results{ ErrorTypeAndDescription{ ETestRunErrorType::Unknown, ""} }, false },
                    std::tuple{ "Zero Failed test run", Results{ TestRunResults{} }, true },
                    std::tuple{ "Non-zero failed test run", Results{ TestRunResults{ {}, {}, {}, 0, 1, uint3{} } }, false }
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
            table<std::string, TestDataSection<StringMetaData>>
            (
                {
                    std::tuple{ "Empty string section", TestDataSection<StringMetaData>{0, 0, 0} },
                    std::tuple{ "Begin: 0, Num: 1, Size: 0", TestDataSection<StringMetaData>{0, 1, 0} },
                    std::tuple{ "Begin: 0, Num: 1, Size: 15", TestDataSection<StringMetaData>{0, 1, 16} },
                    std::tuple{ "Begin: 0, Num: 2, Size: 15", TestDataSection<StringMetaData>{0, 2, 16} },
                    std::tuple{ "Begin: 0, Num: 1, Size: 64", TestDataSection<StringMetaData>{0, 1, 64} },
                    std::tuple{ "Begin: 0, Num: 10, Size: 1024", TestDataSection<StringMetaData>{0, 10, 1024} },
                    std::tuple{ "Begin: 32, Num: 1, Size: 0", TestDataSection<StringMetaData>{32, 1, 0} },
                    std::tuple{ "Begin: 32, Num: 1, Size: 15", TestDataSection<StringMetaData>{32, 1, 16} },
                    std::tuple{ "Begin: 32, Num: 2, Size: 15", TestDataSection<StringMetaData>{32, 2, 16} },
                    std::tuple{ "Begin: 32, Num: 1, Size: 64", TestDataSection<StringMetaData>{32, 1, 64} },
                    std::tuple{ "Begin: 32, Num: 10, Size: 1024", TestDataSection<StringMetaData>{32, 10, 1024} }
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
                auto result = ProcessStrings(stringSection, static_cast<u32>(strings.size()), EncodeStringData(stringSection, strings));

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
            table<std::string, TestDataSection<SectionInfoMetaData>>
            (
                {
                    std::tuple{ "Empty section info section", TestDataSection<SectionInfoMetaData>{0u, 0u, 0u} },
                    std::tuple{ "Begin: 0, Num: 1", TestDataSection<SectionInfoMetaData>{0u, 1u, 0u} },
                    std::tuple{ "Begin: 0, Num: 2", TestDataSection<SectionInfoMetaData>{0u, 2u, 0u} },
                    std::tuple{ "Begin: 0, Num: 32", TestDataSection<SectionInfoMetaData>{0u, 32u, 0u} },
                    std::tuple{ "Begin: 32, Num: 1", TestDataSection<SectionInfoMetaData>{32u, 1u, 0u} },
                    std::tuple{ "Begin: 32, Num: 2", TestDataSection<SectionInfoMetaData>{32u, 2u, 0u} },
                    std::tuple{ "Begin: 32, Num: 32", TestDataSection<SectionInfoMetaData>{32u, 32u, 0u} },
                }
            )
        );

        GIVEN(given)
        {

            auto [when, sections] = GENERATE
            (
                table<std::string, std::vector<SectionInfoMetaData>>
                (
                    {
                        std::tuple{"no sections", std::vector<SectionInfoMetaData>{}},
                        std::tuple{"a single section", std::vector<SectionInfoMetaData>{{0u, -1,-1}}},
                        std::tuple{"Two sections", std::vector<SectionInfoMetaData>{{0u, -1, -1}, {1u, -1, -1}} },
                        std::tuple{"32 sections", std::vector<SectionInfoMetaData>(32, SectionInfoMetaData{0u, -1, -1})}
                    }
                )
            );

            WHEN(when)
            {
                auto result = ProcessSectionInfo(sectionInfo, static_cast<u32>(sections.size()), EncodeSectionData(sectionInfo, sections));

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
            table<std::string, AllocationBufferData>
            (
                {
                    std::tuple{ "Zero success and zero failing asserts", AllocationBufferData{0, 0} },
                    std::tuple{ "Non-Zero success and zero failing asserts", AllocationBufferData{1, 0} },
                    std::tuple{ "Non-Zero success and non-zero failing asserts", AllocationBufferData{1, 1} },
                    std::tuple{ "Zero success and non-zero failing asserts", AllocationBufferData{0, 1} }
                }
            )
        );

        GIVEN(given)
        {
            WHEN("Processed")
            {
                const auto results = ProcessTestDataBuffer(allocationBufferData, uint3(1, 1, 1), TestDataBufferLayout{ 0,0 }, std::vector<std::byte>{}, MultiTypeByteReaderMap{});

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
            table<std::string, u32, u64, TestDataBufferLayout, std::vector<HLSLAssertMetaData>>
            (
                {
                    std::tuple{ "One failing assert with enough capacity", 1, 1, TestDataBufferLayout{1, 0}, std::vector<HLSLAssertMetaData>{ HLSLAssertMetaData{ 42, 0, 1 } } },
                    std::tuple{ "More failing asserts than layout limit", 2, 1, TestDataBufferLayout{1, 0}, std::vector<HLSLAssertMetaData>{ HLSLAssertMetaData{ 42, 0, 1 } } },
                    std::tuple{ "Fewer failing asserts than layout limit", 1, 1, TestDataBufferLayout{2, 0}, std::vector<HLSLAssertMetaData>{ HLSLAssertMetaData{ 42, 0, 1 }, HLSLAssertMetaData{ 82, 0, 1 } } }
                }
            )
        );

        GIVEN(given)
        {
            WHEN("Processed")
            {
                const auto results = ProcessTestDataBuffer(AllocationBufferData{ 0, numFailed }, uint3(1, 1, 1), layout, std::as_bytes(std::span{ buffer }), MultiTypeByteReaderMap{});

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
            table<std::string, u32, u64, TestDataBufferLayout, std::vector<HLSLAssertMetaData>, std::vector<u32>>
            (
                {
                    std::tuple
                    {
                        "One failing assert with data",
                        1, 1, TestDataBufferLayout{1, 100},
                        std::vector{ HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(HLSLAssertMetaData) * 1, 8 } },
                        std::vector{4u}
                    },
                    std::tuple
                    {
                        "One failing assert with no data",
                        1, 1, TestDataBufferLayout{1, 100},
                        std::vector{ HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(HLSLAssertMetaData) * 0, 0 } },
                        std::vector<u32>{}
                    },
                    std::tuple
                    {
                        "Two failing assert with no data",
                        2, 2, TestDataBufferLayout{2, 100},
                        std::vector{ HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(HLSLAssertMetaData) * 0, 0 }, HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(HLSLAssertMetaData) * 0, 0 } },
                        std::vector<u32>{}
                    },
                    std::tuple
                    {
                        "First fail has data, Second does not",
                        2, 2, TestDataBufferLayout{2, 100},
                        std::vector{ HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(HLSLAssertMetaData) * 2, 8 }, HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(HLSLAssertMetaData) * 0, 0 } },
                        std::vector{16u}
                    },
                    std::tuple
                    {
                        "First fail does not have data, Second does",
                        2, 2, TestDataBufferLayout{2, 100},
                        std::vector{ HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(HLSLAssertMetaData) * 0, 0 }, HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(HLSLAssertMetaData) * 2, 8 } },
                        std::vector{16u}
                    },
                    std::tuple
                    {
                        "Both asserts have data",
                        2, 2, TestDataBufferLayout{2, 100},
                        std::vector{ HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(HLSLAssertMetaData) * 2, 8 }, HLSLAssertMetaData{ 42, 0, 0, 0, sizeof(HLSLAssertMetaData) * 2 + 8, 8 } },
                        std::vector{16u, 32u}
                    }
                }
            )
        );

        std::vector<std::byte> buffer;
        auto assertData = EncodeAssertData(expectedAssertData);
        const u64 sizeMetaData = metaData.size() * sizeof(HLSLAssertMetaData);
        buffer.resize(sizeMetaData + assertData.size());

        std::memcpy(buffer.data(), metaData.data(), sizeMetaData);
        std::memcpy(buffer.data() + sizeMetaData, assertData.data(), assertData.size());

        GIVEN(given)
        {
            WHEN("Processed")
            {
                const auto results = ProcessTestDataBuffer(AllocationBufferData{ 0, numFailed }, uint3(1, 1, 1), layout, std::as_bytes(std::span{ buffer }), MultiTypeByteReaderMap{});

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
            table<std::string, u32, TestDataBufferLayout, std::vector<HLSLAssertMetaData>, std::vector<u32>, std::vector<std::string>>
            (
                {
                    std::tuple
                    {
                        "One failing assert with no byte reader",
                        1, TestDataBufferLayout{1, 100},
                        std::vector{ HLSLAssertMetaData{ 42, 0, 0, 0, 0, 2, sizeof(HLSLAssertMetaData) * 1, 8}},
                        std::vector{42u},
                        std::vector<std::string>{"Bytes", "0x2A"}
                    },
                    std::tuple
                    {
                        "One failing assert with byte reader",
                        1, TestDataBufferLayout{1, 100},
                        std::vector{ HLSLAssertMetaData{ 42, 0, 0, 0, 0, 0, sizeof(HLSLAssertMetaData) * 1, 8 } },
                        std::vector{412u},
                        std::vector<std::string>{"Reader 1", "412"}
                    },
                    std::tuple
                    {
                        "One failing assert with different byte reader",
                        1, TestDataBufferLayout{1, 100},
                        std::vector{ HLSLAssertMetaData{ 42, 0, 0, 0, 0, 1, sizeof(HLSLAssertMetaData) * 1, 8 } },
                        std::vector{412u},
                        std::vector<std::string>{"Reader 2", "412"}
                    },
                    std::tuple
                    {
                        "Two failing asserts with same byte reader",
                        2, TestDataBufferLayout{2, 100},
                        std::vector
                        {
                            HLSLAssertMetaData{ 42, 0, 0, 0, 0, 0, sizeof(HLSLAssertMetaData) * 2, 8 },
                            HLSLAssertMetaData{ 42, 0, 0, 0, 0, 0, sizeof(HLSLAssertMetaData) * 2 + 8, 8 }
                        },
                        std::vector{412u, 214u},
                        std::vector<std::string>{"Reader 1", "412", "214"}
                    },
                    std::tuple
                    {
                        "Two failing asserts with different same byte reader",
                        2, TestDataBufferLayout{2, 100},
                        std::vector
                        {
                            HLSLAssertMetaData{ 42, 0, 0, 0, 0, 1, sizeof(HLSLAssertMetaData) * 2, 8 },
                            HLSLAssertMetaData{ 42, 0, 0, 0, 0, 1, sizeof(HLSLAssertMetaData) * 2 + 8, 8 }
                        },
                        std::vector{412u, 214u},
                        std::vector<std::string>{"Reader 2", "412", "214"}
                    },
                    std::tuple
                    {
                        "Two failing asserts with different byte readers",
                        2, TestDataBufferLayout{2, 100},
                        std::vector
                        {
                            HLSLAssertMetaData{ 42, 0, 0, 0, 0, 0, sizeof(HLSLAssertMetaData) * 2, 8 },
                            HLSLAssertMetaData{ 42, 0, 0, 0, 0, 1, sizeof(HLSLAssertMetaData) * 2 + 8, 8 }
                        },
                        std::vector{412u, 214u},
                        std::vector<std::string>{"Reader 1", "Reader 2", "412", "214"}
                    }
                }
            )
        );

        std::vector<std::byte> buffer;
        auto assertData = EncodeAssertData(expectedAssertData);
        const u64 sizeMetaData = metaData.size() * sizeof(HLSLAssertMetaData);
        buffer.resize(sizeMetaData + assertData.size());

        std::memcpy(buffer.data(), metaData.data(), sizeMetaData);
        std::memcpy(buffer.data() + sizeMetaData, assertData.data(), assertData.size());

        GIVEN(given)
        {
            WHEN("Processed")
            {
                const auto results = ProcessTestDataBuffer(AllocationBufferData{ 0, numFailed }, uint3(1, 1, 1), layout, std::as_bytes(std::span{ buffer }), MultiTypeByteReaderMap{ conv1, conv2 });

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
            table<std::string, u32, u64, TestDataBufferLayout, std::vector<HLSLAssertMetaData>>
            (
                {
                    std::tuple{ "One failing assert with empty buffer", 1, 1, TestDataBufferLayout{1, 0}, std::vector<HLSLAssertMetaData>{} },
                    std::tuple{ "Fewer failing asserts than layout limit", 3, 1, TestDataBufferLayout{3, 0}, std::vector<HLSLAssertMetaData>{ HLSLAssertMetaData{ 42, 0, 1 }, HLSLAssertMetaData{ 82, 0, 1 } } }
                }
            )
        );

        GIVEN(given)
        {
            WHEN("Processed")
            {
                THEN("throws")
                {
                    REQUIRE_THROWS(ProcessTestDataBuffer(AllocationBufferData{ 0, numFailed }, uint3(1, 1, 1), layout, std::as_bytes(std::span{ buffer }), MultiTypeByteReaderMap{}));
                }
            }
        }
    }

    SCENARIO("TestDataBufferProcessorTests - FailedAssert - Equality")
    {

        auto [given, left, right, expectedIsEqual] = GENERATE
        (
            table<std::string, FailedAssert, FailedAssert, bool>
            (
                {
                    std::tuple
                    {
                        "Both sides are default constructed",
                        FailedAssert{},
                        FailedAssert{},
                        true
                    },
                    std::tuple
                    {
                        "Differing data",
                        FailedAssert{std::vector{std::byte{1}, std::byte{2}}, {}, {}},
                        FailedAssert{},
                        false
                    },
                    std::tuple
                    {
                        "Differing ByteReaders",
                        FailedAssert{{}, [](const u16, const std::span<const std::byte>) { return std::string{}; }, {}},
                        FailedAssert{},
                        true
                    },
                    std::tuple
                    {
                        "Differing Meta data",
                        FailedAssert{{}, {}, {1, 0, 0}},
                        FailedAssert{},
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
            table<std::string, u32, u64, uint3, TestDataBufferLayout, std::vector<HLSLAssertMetaData>, std::vector<std::string>, std::vector<std::string>>
            (
                {
                    std::tuple
                    {
                        "No thread id type and zero id",
                        1, 1, uint3(10, 10, 10),
                        TestDataBufferLayout{1, 0},
                        std::vector{ HLSLAssertMetaData{ 42, 0, 0 } },
                        std::vector<std::string>{"line"},
                        std::vector<std::string>{"threadid"}
                    },
                    std::tuple
                    {
                        "No thread id type and non zero id",
                        1, 1, uint3(10, 10, 10),
                        TestDataBufferLayout{1, 0},
                        std::vector{ HLSLAssertMetaData{ 42, 34, 0 } },
                        std::vector<std::string>{"line"},
                        std::vector<std::string>{"threadid"}
                    },
                    std::tuple
                    {
                        "int thread id type and zero id",
                        1, 1, uint3(10, 10, 10),
                        TestDataBufferLayout{1, 0},
                        std::vector{ HLSLAssertMetaData{ 42, 0, 1 } },
                        std::vector<std::string>{"0"},
                        std::vector<std::string>{}
                    },
                    std::tuple
                    {
                        "int thread id type and non zero id",
                        1, 1, uint3(10, 10, 10),
                        TestDataBufferLayout{1, 0},
                        std::vector{ HLSLAssertMetaData{ 42, 34, 1 } },
                        std::vector<std::string>{"34"},
                        std::vector<std::string>{}
                    },
                    std::tuple
                    {
                        "int3 thread id type and zero id",
                        1, 1, uint3(10, 10, 10),
                        TestDataBufferLayout{1, 0},
                        std::vector{ HLSLAssertMetaData{ 42, 0, 2 } },
                        std::vector<std::string>{"0"},
                        std::vector<std::string>{}
                    },
                    std::tuple
                    {
                        "int3 thread id type and non zero id",
                        1, 1, uint3(10, 10, 10),
                        TestDataBufferLayout{1, 0},
                        std::vector{ HLSLAssertMetaData{ 42, 34, 2 } },
                        std::vector<std::string>{"4, 3, 0"},
                        std::vector<std::string>{}
                    },
                    std::tuple
                    {
                        "int3 thread id type and different non zero id",
                        1, 1, uint3(10, 10, 10),
                        TestDataBufferLayout{1, 0},
                        std::vector{ HLSLAssertMetaData{ 42, 342, 2 } },
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
                const auto results = ProcessTestDataBuffer(AllocationBufferData{ 0, numFailed }, dims, layout, std::as_bytes(std::span{ buffer }), MultiTypeByteReaderMap{});

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

    SCENARIO("TestDataBufferProcessorTests - TestRunResults - Stream Operator")
    {
        using Catch::Matchers::ContainsSubstring;
        using Catch::Matchers::Equals;

        GIVEN("Zero asserts, zero strings and zero sections")
        {
            auto [numPass, numFail, expectedString] = GENERATE
            (
                table<u32, u32, std::string>
                (
                    {
                        std::tuple
                        {
                            0u,
                            0u,
                            "There were 0 successful asserts and 0 failed assertions\n"
                        },
                        std::tuple
                        {
                            1u,
                            0u,
                            "There were 1 successful asserts and 0 failed assertions\n"
                        },
                        std::tuple
                        {
                            2u,
                            0u,
                            "There were 2 successful asserts and 0 failed assertions\n"
                        },
                        std::tuple
                        {
                            0u,
                            1u,
                            "There were 0 successful asserts and 1 failed assertions\n"
                        },
                        std::tuple
                        {
                            0u,
                            2u,
                            "There were 0 successful asserts and 2 failed assertions\n"
                        },
                        std::tuple
                        {
                            42u,
                            125u,
                            "There were 42 successful asserts and 125 failed assertions\n"
                        }
                    }
                )
            );

            WHEN("Num Passes: " << numPass << " Num Fails: " << numFail)
            {
                std::stringstream buffer;
                const TestRunResults results{ .NumSucceeded = numPass, .NumFailed = numFail };

                buffer << results;

                THEN("Output is as expected")
                {
                    REQUIRE_THAT(buffer.str(), Equals(expectedString, Catch::CaseSensitive::No));
                }
            }
        }

        GIVEN("Failed Asserts with no data")
        {
            auto [andGiven, failedAsserts, sections, strings, expectedSubstrings, expectedAbsentSubstrings ] = GENERATE
            (
                table<std::string, std::vector<FailedAssert>, std::vector<SectionInfoMetaData>, std::vector<std::string>, std::vector<std::string>, std::vector<std::string>>
                (
                    {
                        std::tuple
                        {
                            "single assert with no sections and no strings and no threadId",
                            std::vector
                            {
                                FailedAssert
                                {
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 42,
                                        .ThreadId = 0,
                                        .ThreadIdType = 0,
                                        .SectionId = -1
                                    }
                                }
                            },
                            std::vector<SectionInfoMetaData>{},
                            std::vector<std::string>{},
                            std::vector<std::string>{"Assert 0", "Line: 42"},
                            std::vector<std::string>{"INVALID SECTION ID", "SECTION", "SCENARIO", "UNKNOWN", "DATA"}
                        },
                        std::tuple
                        {
                            "single assert with no sections and no strings and no threadId with data",
                            std::vector
                            {
                                FailedAssert
                                {
                                    .Data = EncodeAssertData(std::vector<u32>{0x12345678u}),
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 42,
                                        .ThreadId = 0,
                                        .ThreadIdType = 0,
                                        .SectionId = -1
                                    }
                                }
                            },
                            std::vector<SectionInfoMetaData>{},
                            std::vector<std::string>{},
                            std::vector<std::string>{"Assert 0", "Line: 42"},
                            std::vector<std::string>{"INVALID SECTION ID", "SECTION", "SCENARIO", "UNKNOWN", "DATA"}
                        },
                        std::tuple
                        {
                            "single assert with no sections and no strings and no threadId with data and Byte reader",
                            std::vector
                            {
                                FailedAssert
                                {
                                    .Data = EncodeAssertData(std::vector<u32>{1234}),
                                    .ByteReader = CreateByteReader("Expected Byte Reader"),
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 42,
                                        .ThreadId = 0,
                                        .ThreadIdType = 0,
                                        .SectionId = -1
                                    }
                                }
                            },
                            std::vector<SectionInfoMetaData>{},
                            std::vector<std::string>{},
                            std::vector<std::string>{"Assert 0", "Line: 42", "Expected Byte Reader", "Data 1:", "Type 0"},
                            std::vector<std::string>{"INVALID SECTION ID", "SECTION", "SCENARIO", "UNKNOWN", "Data 2: "}
                        },
                        std::tuple
                        {
                            "single assert with no sections and no strings and no threadId with data and Byte reader and type id",
                            std::vector
                            {
                                FailedAssert
                                {
                                    .Data = EncodeAssertData(std::vector<u32>{1234}),
                                    .ByteReader = CreateByteReader("Expected Byte Reader"),
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 42,
                                        .ThreadId = 0,
                                        .ThreadIdType = 0,
                                        .SectionId = -1
                                    },
                                    .TypeId = 21
                                }
                            },
                            std::vector<SectionInfoMetaData>{},
                            std::vector<std::string>{},
                            std::vector<std::string>{"Assert 0", "Line: 42", "Expected Byte Reader", "Data 1:", "Type 21"},
                            std::vector<std::string>{"INVALID SECTION ID", "SECTION", "SCENARIO", "UNKNOWN", "Data 2: ", "Type 0"}
                        },
                        std::tuple
                        {
                            "single assert with no sections and no strings and no threadId with mulitple data and Byte reader and type id",
                            std::vector
                            {
                                FailedAssert
                                {
                                    .Data = EncodeAssertData(std::vector{Tuple{1234, 5678}}),
                                    .ByteReader = CreateByteReader("Expected Byte Reader"),
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 42,
                                        .ThreadId = 0,
                                        .ThreadIdType = 0,
                                        .SectionId = -1
                                    },
                                    .TypeId = 21
                                }
                            },
                            std::vector<SectionInfoMetaData>{},
                            std::vector<std::string>{},
                            std::vector<std::string>{"Assert 0", "Line: 42", "Expected Byte Reader", "Data 1:", "Data 2: ", "Type 21", "1234", "5678"},
                            std::vector<std::string>{"INVALID SECTION ID", "SECTION", "SCENARIO", "UNKNOWN" "Type 0"}
                        },
                        std::tuple
                        {
                            "single assert with no sections and no strings and single threadId",
                            std::vector
                            {
                                FailedAssert
                                {
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 42,
                                        .ThreadId = 34,
                                        .ThreadIdType = 1,
                                        .SectionId = -1
                                    }
                                }
                            },
                            std::vector<SectionInfoMetaData>{},
                            std::vector<std::string>{},
                            std::vector<std::string>{"Assert 0", "Line: 42", "ThreadId: 34"},
                            std::vector<std::string>{"INVALID SECTION ID", "SECTION", "SCENARIO", "UNKNOWN", "DATA"}
                        },
                        std::tuple
                        {
                            "single assert with no sections and no strings and multi dim threadId",
                            std::vector
                            {
                                FailedAssert
                                {
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 42,
                                        .ThreadId = 34,
                                        .ThreadIdType = 2,
                                        .SectionId = -1
                                    }
                                }
                            },
                            std::vector<SectionInfoMetaData>{},
                            std::vector<std::string>{},
                            std::vector<std::string>{"Assert 0", "Line: 42", "ThreadId: (34, 0, 0)"},
                            std::vector<std::string>{"INVALID SECTION ID", "SECTION", "SCENARIO", "UNKNOWN", "DATA"}
                        },
                        std::tuple
                        {
                            "two asserts with no sections and no strings",
                            std::vector
                            {
                                FailedAssert
                                {
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 42,
                                        .ThreadId = 34,
                                        .ThreadIdType = 2,
                                        .SectionId = -1
                                    }
                                },
                                FailedAssert
                                {
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 56,
                                        .ThreadId = 16,
                                        .ThreadIdType = 1,
                                        .SectionId = -1
                                    }
                                }
                            },
                            std::vector<SectionInfoMetaData>{},
                            std::vector<std::string>{},
                            std::vector<std::string>{"Assert 0", "Assert 1", "Line: 42", "Line: 56", "ThreadId: 16"},
                            std::vector<std::string>{"INVALID SECTION ID", "SECTION", "SCENARIO", "UNKNOWN", "DATA"}
                        },
                        std::tuple
                        {
                            "single assert with a section and no strings",
                            std::vector
                            {
                                FailedAssert
                                {
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 42,
                                        .ThreadId = 0,
                                        .ThreadIdType = 0,
                                        .SectionId = 0
                                    }
                                }
                            },
                            std::vector
                            {
                                SectionInfoMetaData
                                {
                                }
                            },
                            std::vector<std::string>{},
                            std::vector<std::string>{"Assert 0", "Line: 42", "SCENARIO", "UNKNOWN"},
                            std::vector<std::string>{"INVALID SECTION ID", "SECTION", "DATA", "\t\t", "\t"}
                        },
                        std::tuple
                        {
                            "single assert with invalid section tree and no strings",
                            std::vector
                            {
                                FailedAssert
                                {
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 42,
                                        .ThreadId = 0,
                                        .ThreadIdType = 0,
                                        .SectionId = 0
                                    }
                                }
                            },
                            std::vector
                            {
                                SectionInfoMetaData
                                {
                                    .ParentId = 1
                                }
                            },
                            std::vector<std::string>{},
                            std::vector<std::string>{"Assert 0", "Line: 42", "INVALID SECTION ID" },
                            std::vector<std::string>{"SCENARIO", "UNKNOWN", "DATA"}
                        },
                        std::tuple
                        {
                            "single assert with section that does not exist and no strings",
                            std::vector
                            {
                                FailedAssert
                                {
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 42,
                                        .ThreadId = 0,
                                        .ThreadIdType = 0,
                                        .SectionId = 1
                                    }
                                }
                            },
                            std::vector
                            {
                                SectionInfoMetaData
                                {
                                }
                            },
                            std::vector<std::string>{},
                            std::vector<std::string>{"Assert 0", "Line: 42" },
                            std::vector<std::string>{"INVALID SECTION ID", "SECTION", "SCENARIO", "UNKNOWN", "DATA"}
                        },
                        std::tuple
                        {
                            "single assert with a multi section tree",
                            std::vector
                            {
                                FailedAssert
                                {
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 42,
                                        .ThreadId = 0,
                                        .ThreadIdType = 0,
                                        .SectionId = 1
                                    }
                                }
                            },
                            std::vector
                            {
                                SectionInfoMetaData
                                {
                                },
                                SectionInfoMetaData
                                {
                                    .ParentId = 0
                                }
                            },
                            std::vector<std::string>{},
                            std::vector<std::string>{"Assert 0", "Line: 42", "SCENARIO", "UNKNOWN", "SECTION", "\t"},
                            std::vector<std::string>{"INVALID SECTION ID", "DATA", "\t\t\t", "\t\t"}
                        },
                        std::tuple
                        {
                            "single assert with a multi section tree and assert data",
                            std::vector
                            {
                                FailedAssert
                                {
                                    .Data = EncodeAssertData(std::vector{34u}),
                                    .ByteReader = CreateByteReader("Expected"),
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 42,
                                        .ThreadId = 0,
                                        .ThreadIdType = 0,
                                        .SectionId = 1
                                    }
                                }
                            },
                            std::vector
                            {
                                SectionInfoMetaData
                                {
                                },
                                SectionInfoMetaData
                                {
                                    .ParentId = 0
                                }
                            },
                            std::vector<std::string>{},
                            std::vector<std::string>{"Assert 0", "Line: 42", "SCENARIO", "UNKNOWN", "\tSECTION", "\t\tData 1:", "Expected"},
                            std::vector<std::string>{"INVALID SECTION ID", "\t\t\t"}
                        },
                        std::tuple
                        {
                            "single assert with a section with a valid string id",
                            std::vector
                            {
                                FailedAssert
                                {
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 42,
                                        .ThreadId = 0,
                                        .ThreadIdType = 0,
                                        .SectionId = 0
                                    }
                                }
                            },
                            std::vector
                            {
                                SectionInfoMetaData
                                {
                                    .StringId = 0
                                }
                            },
                            std::vector<std::string>{"TestingString"},
                            std::vector<std::string>{"Assert 0", "Line: 42", "SCENARIO", "TestingString"},
                            std::vector<std::string>{"INVALID SECTION ID", "SECTION", "DATA", "UNKNOWN"}
                        }, 
                        std::tuple
                        {
                            "single assert with a multi section tree with valid string ids",
                            std::vector
                            {
                                FailedAssert
                                {
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 42,
                                        .ThreadId = 0,
                                        .ThreadIdType = 0,
                                        .SectionId = 1
                                    }
                                }
                            },
                            std::vector
                            {
                                SectionInfoMetaData
                                {
                                    .StringId = 0
                                },
                                SectionInfoMetaData
                                {
                                    .StringId = 1,
                                    .ParentId = 0
                                }
                            },
                            std::vector<std::string>{"TestingString1", "TestingString2"},
                            std::vector<std::string>{"Assert 0", "Line: 42", "SCENARIO: TestingString1", "SECTION: TestingString2"},
                            std::vector<std::string>{"INVALID SECTION ID", "DATA", "UNKNOWN"}
                        },
                        std::tuple
                        {
                            "single assert with a section with an invalid string id",
                            std::vector
                            {
                                FailedAssert
                                {
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 42,
                                        .ThreadId = 0,
                                        .ThreadIdType = 0,
                                        .SectionId = 0
                                    }
                                }
                            },
                            std::vector
                            {
                                SectionInfoMetaData
                                {
                                    .StringId = 1
                                }
                            },
                            std::vector<std::string>{"TestingString"},
                            std::vector<std::string>{"Assert 0", "Line: 42", "SCENARIO", "UNKNOWN"},
                            std::vector<std::string>{"INVALID SECTION ID", "SECTION", "DATA", "TestingString"}
                        },
                        std::tuple
                        {
                            "single assert with a multi section tree with 1 invalid string id",
                            std::vector
                            {
                                FailedAssert
                                {
                                    .Info = AssertMetaData
                                    {
                                        .LineNumber = 42,
                                        .ThreadId = 0,
                                        .ThreadIdType = 0,
                                        .SectionId = 1
                                    }
                                }
                            },
                            std::vector
                            {
                                SectionInfoMetaData
                                {
                                    .StringId = 0
                                },
                                SectionInfoMetaData
                                {
                                    .StringId = 2,
                                    .ParentId = 0
                                }
                            },
                            std::vector<std::string>{"TestingString1", "TestingString2"},
                            std::vector<std::string>{"Assert 0", "Line: 42", "SCENARIO: TestingString1", "SECTION: UNKNOWN"},
                            std::vector<std::string>{"INVALID SECTION ID", "DATA"}
                        }
                    }
                )
            );

            AND_GIVEN(andGiven)
            {
                WHEN("results streamed")
                {
                    std::stringstream buffer;

                    const TestRunResults results{ .FailedAsserts = failedAsserts, .Strings = std::move(strings), .Sections = std::move(sections), .DispatchDimensions = uint3(100,1,1) };

                    buffer << results;

                    THEN("Output is as expected")
                    {
                        for (const auto& expectedString : expectedSubstrings)
                        {
                            REQUIRE_THAT(buffer.str(), ContainsSubstring(expectedString, Catch::CaseSensitive::No));
                        }

                        for (const auto& expectedString : expectedAbsentSubstrings)
                        {
                            REQUIRE_THAT(buffer.str(), !ContainsSubstring(expectedString, Catch::CaseSensitive::No));
                        }
                    }
                }
            }
        }
    }
}
