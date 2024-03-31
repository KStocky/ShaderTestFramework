#include "Framework/TestDataBufferProcessor.h"

#include "Utility/Exception.h"
#include "Utility/Math.h"

#include <format>
#include <ranges>
#include <sstream>

namespace STF
{
    static_assert(sizeof(HLSLAssertMetaData) == 24,
        "The size of this struct must be the same size as ShaderTestPrivate::HLSLAssertMetaData");

    static uint3 Unflatten(const u32 InId, const uint3 InDims)
    {
        const u32 xyDim = InDims.x * InDims.y;
        const u32 z = InId / xyDim;
        const u32 xy = InId - (z * xyDim);
        return uint3{ xy % InDims.x, xy / InDims.x, z };
    }

    static std::string ThreadInfoToString(const STF::EThreadIdType InType, const u32 InId, const uint3 InDispatchDimensions)
    {
        switch (InType)
        {
            case STF::EThreadIdType::None:
            {
                return "Thread Id not initialized for test";
            }

            case STF::EThreadIdType::Int:
            {
                return std::format("ThreadId: {}", InId);
            }

            case STF::EThreadIdType::Int3:
            {
                return std::format("ThreadId: {}", Unflatten(InId, InDispatchDimensions));
            }
        }

        return "";
    }

    Results::Results(FailedShaderCompilationResult InError)
        : m_Result(std::move(InError))
    {
    }

    Results::Results(TestRunResults InResults)
        : m_Result(std::move(InResults))
    {
    }

    const TestRunResults* Results::GetTestResults() const
    {
        return std::get_if<TestRunResults>(&m_Result);
    }

    const FailedShaderCompilationResult* Results::GetFailedCompilationResult() const
    {
		return std::get_if<FailedShaderCompilationResult>(&m_Result);
    }

    std::string DefaultByteReader(const u16, std::span<const std::byte> InBytes)
    {
        std::string ret;

        if (InBytes.empty() || (InBytes.size() % 4 != 0))
        {
            return ret;
        }

        auto toUint = [](const auto InChunk)
            {
                u32 ret = 0;
                std::memcpy(&ret, InChunk.data(), sizeof(u32));
                return ret;
            };

        std::format_to(std::back_inserter(ret), "Bytes: {:#x}", toUint(InBytes));

        for (const auto& byte : InBytes | std::views::drop(4) | std::views::chunk(4))
        {
            std::format_to(std::back_inserter(ret), ", {:#x}", toUint(byte));
        }

        return ret;
    }

    bool operator==(const FailedAssert& InA, const FailedAssert& InB)
    {
        return InA.Data == InB.Data && InA.Info == InB.Info && InA.TypeId == InB.TypeId;
    }

    bool operator!=(const FailedAssert& InA, const FailedAssert& InB)
    {
        return !(InA == InB);
    }

    std::ostream& operator<<(std::ostream& InOs, const TestRunResults& In)
    {
        InOs << std::format("There were {} successful asserts and {} failed assertions\n", In.NumSucceeded, In.NumFailed);

        if (In.FailedAsserts.empty())
        {
            return InOs;
        }

        for (const auto& [index, error] : std::views::enumerate(In.FailedAsserts))
        {
            const std::string lineInfo = error.Info.LineNumber == u32(-1) ? std::string{ "" } : std::format("Line: {}", error.Info.LineNumber);
            const std::string threadInfo = error.Info.ThreadIdType == 0 ? std::string{ "" } : STF::ThreadInfoToString(static_cast<STF::EThreadIdType>(error.Info.ThreadIdType), error.Info.ThreadId, In.DispatchDimensions);
            InOs << std::format("Assert {}: {} {}\n", index, lineInfo, threadInfo);

            if (error.Data.size() == 0 || !error.ByteReader)
            {
                continue;
            }

            u64 byteIndex = 0;
            const auto endIndex = error.Data.size();

            for (u32 i = 0; byteIndex < endIndex; ++i)
            {
                u32 sizeAndAlignData;
                std::memcpy(&sizeAndAlignData, error.Data.data() + byteIndex, sizeof(u32));
                const u32 size = sizeAndAlignData >> 16;
                const u32 align = sizeAndAlignData & 0xffff;
                byteIndex = AlignedOffset(byteIndex + sizeof(4), align);

                InOs << std::format("Data {}: {}\n", i + 1u, error.ByteReader(error.TypeId, std::span{ error.Data.cbegin() + byteIndex, size }));
                byteIndex = AlignedOffset(byteIndex + size, 4);
            }
        }

        return InOs;
    }

    std::ostream& operator<<(std::ostream& InOs, const FailedShaderCompilationResult& In)
    {
        InOs << In.Error;
        return InOs;
    }

    std::ostream& operator<<(std::ostream& InOs, const Results& In)
    {
        std::visit(
            OverloadSet
            {
                [&InOs](std::monostate)
                {
                    InOs << "Results not initialized";
                },
                [&InOs](const TestRunResults& InTestResults)
                {
                    InOs << InTestResults;
                },
                [&InOs](const FailedShaderCompilationResult& InCompilationError)
                {
                    InOs << InCompilationError;
                }
            }, In.m_Result);

        return InOs;
    }

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

    TestRunResults ProcessTestDataBuffer(const AllocationBufferData InAllocationBufferData, const uint3 InDispatchDimensions, const TestDataBufferLayout InLayout, std::span<const std::byte> InTestData, const MultiTypeByteReaderMap& InByteReaderMap)
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
