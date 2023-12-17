#include "Framework/AssertBufferProcessor.h"

#include "Utility/Exception.h"

#include <format>
#include <ranges>

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

    Results::Results(std::string InError)
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

    std::ostream& operator<<(std::ostream& InOs, const STF::Results& In)
    {
        std::visit(
            OverloadSet
            {
                [&InOs](std::monostate)
                {
                    InOs << "Results not initialized";
                },
                [&InOs](const STF::TestRunResults& InTestResults)
                {
                    InOs << std::format("There were {} successful asserts and {} failed assertions\n", InTestResults.NumSucceeded, InTestResults.NumFailed);

                    for (const auto& [index, error] : std::views::enumerate(InTestResults.FailedAsserts))
                    {
                        InOs << std::format("{}: Line: {} {}\n", index, error.Info.LineNumber, STF::ThreadInfoToString(static_cast<STF::EThreadIdType>(error.Info.ThreadIdType), error.Info.ThreadId, InTestResults.DispatchDimensions));
                    }
                },
                [&InOs](const std::string& InCompilationError)
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
    const TypeConverterMap&)
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
        
        ret.FailedAsserts.push_back(FailedAssert{ .Data = {}, .Info = assertInfo });
    }

    return ret;
}
