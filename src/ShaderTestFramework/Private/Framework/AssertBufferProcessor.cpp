#include "Framework/AssertBufferProcessor.h"

#include "Utility/Exception.h"

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
            std::format_to(std::back_inserter(ret), ", {}", toUint(byte));
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

        for (const auto& [index, error] : std::views::enumerate(In.FailedAsserts))
        {
            InOs << std::format("{}: Line: {} {}\n", index, error.Info.LineNumber, STF::ThreadInfoToString(static_cast<STF::EThreadIdType>(error.Info.ThreadIdType), error.Info.ThreadId, In.DispatchDimensions));

            if (error.Data.size() > 0)
            {
                u32 byteIndex = 0;
                const auto endIndex = error.Data.size();

                while (byteIndex < endIndex)
                {
                    u32 sizeData;
                    std::memcpy(&sizeData, error.Data.data(), sizeof(u32));
                    byteIndex += sizeof(u32);

                    InOs << std::format("Data: {}\n", error.DataToStringConverter(std::span{ error.Data.cbegin() + byteIndex, error.Data.cend() }));
                    byteIndex += sizeData;
                }
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
