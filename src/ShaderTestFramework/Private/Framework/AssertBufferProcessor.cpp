#include "Framework/AssertBufferProcessor.h"

#include "Utility/Exception.h"
#include "Utility/Math.h"

#include <format>
#include <ranges>
#include <sstream>

namespace STF
{
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

    std::string DefaultTypeConverter(std::span<const std::byte> InBytes)
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

        std::format_to(std::back_inserter(ret), "Bytes: {:#x}", toUint(InBytes) );

        for (const auto& byte : InBytes | std::views::drop(4) | std::views::chunk(4))
        {
            std::format_to(std::back_inserter(ret), ", {:#x}", toUint(byte));
        }

        return ret;
    }

    bool operator==(const FailedAssert& InA, const FailedAssert& InB)
    {
        return InA.Data == InB.Data && InA.Info == InB.Info;
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

            if (error.Data.size() == 0 || !error.DataToStringConverter)
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

                InOs << std::format("Data {}: {}\n", i + 1u, error.DataToStringConverter(std::span{ error.Data.cbegin() + byteIndex, size }));
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
}

STF::TestRunResults STF::ProcessAssertBuffer(
    const u32 InNumSuccessful, 
    const u32 InNumFailed,
    const uint3 InDispatchDimensions,
    const AssertBufferLayout InLayout,
    std::span<const std::byte> InAssertBuffer, 
    const TypeConverterMap& InTypeConverterMap)
{
    STF::TestRunResults ret{};
    ret.NumSucceeded = InNumSuccessful;
    ret.NumFailed = InNumFailed;
    ret.DispatchDimensions = InDispatchDimensions;

    const bool hasFailedAssertInfo = InLayout.NumFailedAsserts > 0;
    const bool allSucceeded = InNumFailed == 0;

    if (allSucceeded || !hasFailedAssertInfo)
    {
        return ret;
    }

    const u64 requiredBytesForAssertMetaData = InLayout.NumFailedAsserts * sizeof(HLSLAssertMetaData);
    const bool hasSpaceForAssertInfo = requiredBytesForAssertMetaData <= InAssertBuffer.size_bytes();

    ThrowIfFalse(hasSpaceForAssertInfo, std::format("Assert Buffer is not large enough ({} bytes) to hold {} failed asserts (requires {} bytes)", InAssertBuffer.size_bytes(), InNumFailed, requiredBytesForAssertMetaData));

    const u32 numAssertsToProcess = std::min(InNumFailed, InLayout.NumFailedAsserts);

    ret.FailedAsserts.reserve(numAssertsToProcess);

    for (u32 assertIndex = 0; assertIndex < numAssertsToProcess; ++assertIndex)
    {
        HLSLAssertMetaData assertInfo;
        std::memcpy(&assertInfo, &InAssertBuffer[assertIndex * sizeof(HLSLAssertMetaData)], sizeof(HLSLAssertMetaData));
        AssertMetaData info
        {
            .LineNumber = assertInfo.LineNumber,
            .ThreadId = assertInfo.ThreadId,
            .ThreadIdType = assertInfo.ThreadIdType
        };

        auto typeConverter = assertInfo.TypeId < InTypeConverterMap.size() ? InTypeConverterMap[assertInfo.TypeId] : DefaultTypeConverter;

        auto getData = [InAssertBuffer, &assertInfo]()
        {
            if (assertInfo.DataSize == 0)
            {
                return std::vector<std::byte>{};
            }
            const auto begin = InAssertBuffer.cbegin() + assertInfo.DataAddress;
            return std::vector<std::byte>{begin, begin + assertInfo.DataSize};
        };
        
        ret.FailedAsserts.push_back(FailedAssert{ .Data = getData(), .DataToStringConverter = std::move(typeConverter), .Info = info});
    }

    return ret;
}
