
#include "Framework/TestDataBufferProcessor.h"
#include "Utility/Exception.h"

#include <format>

namespace stf
{
    std::vector<FailedAssert> ProcessFailedAsserts(const TestDataSection<HLSLAssertMetaData>& InAssertSection, const u32 InNumFailed, const std::span<const std::byte> InTestData, const MultiTypeByteReaderMap& InByteReaderMap)
    {
        const u32 numAssertsToProcess = std::min(InNumFailed, InAssertSection.NumMeta());
        std::vector<FailedAssert> ret;
        ret.reserve(numAssertsToProcess);

        for (u32 assertIndex = 0; assertIndex < numAssertsToProcess; ++assertIndex)
        {
            HLSLAssertMetaData assertInfo;
            std::memcpy(&assertInfo, &InTestData[assertIndex * sizeof(HLSLAssertMetaData)], sizeof(HLSLAssertMetaData));

            auto byteReader = assertInfo.ReaderId < InByteReaderMap.size() ? InByteReaderMap[assertInfo.ReaderId] : DefaultByteReader;

            auto getData = [InTestData, &assertInfo]()
                {
                    if (assertInfo.DynamicDataInfo.DataSize == 0)
                    {
                        return std::vector<std::byte>{};
                    }
                    const auto begin = InTestData.cbegin() + assertInfo.DynamicDataInfo.DataAddress;
                    return std::vector<std::byte>{begin, begin + assertInfo.DynamicDataInfo.DataSize};
                };

            ret.push_back(FailedAssert{ .Data = getData(), .ByteReader = std::move(byteReader), .Info = assertInfo, .TypeId = assertInfo.TypeId });
        }

        return ret;
    }

    std::vector<std::string> ProcessStrings(const TestDataSection<StringMetaData>& InStringSection, const u32 InNumStrings, const std::span<const std::byte> InTestData)
    {
        const u32 numStringsToProcess = std::min(InNumStrings, InStringSection.NumMeta());
        std::vector<std::string> ret;
        ret.reserve(numStringsToProcess);

        for (u32 stringIndex = 0; stringIndex < numStringsToProcess; ++stringIndex)
        {
            StringMetaData stringInfo;
            std::memcpy(&stringInfo, &InTestData[InStringSection.Begin() + stringIndex * sizeof(StringMetaData)], sizeof(StringMetaData));

            const bool hasString = stringInfo.DynamicDataInfo.DataAddress + stringInfo.DynamicDataInfo.DataSize > 0;
            const bool hasRoomForString = stringInfo.DynamicDataInfo.DataAddress + stringInfo.DynamicDataInfo.DataSize < InTestData.size_bytes();
            if (!hasString || !hasRoomForString)
            {
                break;
            }

            std::string str;
            str.reserve(stringInfo.DynamicDataInfo.DataSize);

            const u32 numPacks = stringInfo.DynamicDataInfo.DataSize / 4;

            for (u32 packedIndex = 0; packedIndex < numPacks; ++packedIndex)
            {
                u32 packedChars;
                std::memcpy(&packedChars, &InTestData[stringInfo.DynamicDataInfo.DataAddress + packedIndex * 4u], sizeof(u32));

                if (packedIndex != numPacks - 1)
                {
                    str.push_back(static_cast<char>((packedChars >> 0) & 0xFF));
                    str.push_back(static_cast<char>((packedChars >> 8) & 0xFF));
                    str.push_back(static_cast<char>((packedChars >> 16) & 0xFF));
                    str.push_back(static_cast<char>((packedChars >> 24) & 0xFF));
                }
                else
                {
                    while (packedChars != 0)
                    {
                        str.push_back(static_cast<char>(packedChars & 0xFF));
                        packedChars = packedChars >> 8;
                    }
                }
            }

            ret.push_back(std::move(str));
        }

        return ret;
    }

    std::vector<SectionInfoMetaData> ProcessSectionInfo(const TestDataSection<SectionInfoMetaData>& InSectionInfoSection, const u32 InNumSections, const std::span<const std::byte> InTestData)
    {
        const u32 numSectionsToProcess = std::min(InNumSections, InSectionInfoSection.NumMeta());
        std::vector<SectionInfoMetaData> ret;
        ret.reserve(numSectionsToProcess);

        for (u32 sectionIndex = 0; sectionIndex < numSectionsToProcess; ++sectionIndex)
        {
            SectionInfoMetaData sectionInfo;
            std::memcpy(&sectionInfo, &InTestData[InSectionInfoSection.Begin() + sectionIndex * sizeof(SectionInfoMetaData)], sizeof(SectionInfoMetaData));

            ret.push_back(sectionInfo);
        }

        return ret;
    }

    TestRunResults ProcessTestDataBuffer(const AllocationBufferData InAllocationBufferData, const uint3 InDispatchDimensions, const TestDataBufferLayout& InLayout, std::span<const std::byte> InTestData, const MultiTypeByteReaderMap& InByteReaderMap)
    {
        TestRunResults ret{};
        ret.NumSucceeded = InAllocationBufferData.NumPassedAsserts;
        ret.NumFailed = InAllocationBufferData.NumFailedAsserts;
        ret.DispatchDimensions = InDispatchDimensions;

        const auto assertSectionInfo = InLayout.GetAssertSection();
        const auto stringSectionInfo = InLayout.GetStringSection();
        const auto sectionInfoSectionInfo = InLayout.GetSectionInfoSection();

        const bool hasFailedAssertInfo = assertSectionInfo.NumMeta() > 0;
        const bool allSucceeded = ret.NumFailed == 0;

        if (allSucceeded || !hasFailedAssertInfo)
        {
            return ret;
        }

        const u64 requiredBytesForMetaData = assertSectionInfo.SizeInBytesOfMeta() + stringSectionInfo.SizeInBytesOfMeta() + sectionInfoSectionInfo.SizeInBytesOfMeta();
        const bool hasSpaceForMetaInfo = requiredBytesForMetaData <= InTestData.size_bytes();

        ThrowIfFalse(hasSpaceForMetaInfo, std::format("Assert Buffer is not large enough ({} bytes) to hold {} failed asserts (requires {} bytes)", InTestData.size_bytes(), InAllocationBufferData.NumFailedAsserts, requiredBytesForMetaData));

        ret.FailedAsserts = ProcessFailedAsserts(assertSectionInfo, ret.NumFailed, InTestData, InByteReaderMap);
        ret.Strings = ProcessStrings(stringSectionInfo, InAllocationBufferData.NumStrings, InTestData);
        ret.Sections = ProcessSectionInfo(sectionInfoSectionInfo, InAllocationBufferData.NumSections, InTestData);

        return ret;
    }
}
