
#include "Framework/ShaderTestCommon.h"

#include "Utility/EnumReflection.h"
#include "Utility/OverloadSet.h"

#include <ranges>

namespace stf
{
    static uint3 Unflatten(const u32 InId, const uint3 InDims)
    {
        const u32 xyDim = InDims.x * InDims.y;
        const u32 z = InId / xyDim;
        const u32 xy = InId - (z * xyDim);
        return uint3{ xy % InDims.x, xy / InDims.x, z };
    }

    static std::string ThreadInfoToString(const EThreadIdType InType, const u32 InId, const uint3 InDispatchDimensions)
    {
        switch (InType)
        {
            case EThreadIdType::None:
            {
                return "Thread Id not initialized for test";
            }

            case EThreadIdType::Int:
            {
                return std::format("ThreadId: {}", InId);
            }

            case EThreadIdType::Int3:
            {
                return std::format("ThreadId: {}", Unflatten(InId, InDispatchDimensions));
            }
        }

        return "";
    }

    Results::Results(ErrorTypeAndDescription InError)
        : m_Result(std::move(InError))
    {
    }

    Results::Results(TestRunResults InResults)
        : m_Result(std::move(InResults))
    {
    }

    Results::operator bool() const
    {
        return std::visit(OverloadSet{
        [](std::monostate)
        {
            return false;
        },
        [](const TestRunResults& InTestResults)
        {
            return InTestResults.NumFailed == 0;
        },
        [](const ErrorTypeAndDescription&)
        {
            return false;
        } }, m_Result);
    }

    const TestRunResults* Results::GetTestResults() const
    {
        return std::get_if<TestRunResults>(&m_Result);
    }

    const ErrorTypeAndDescription* Results::GetTestRunError() const
    {
        return std::get_if<ErrorTypeAndDescription>(&m_Result);
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
        using namespace std::literals;
        InOs << std::format("There were {} successful asserts and {} failed assertions\n", In.NumSucceeded, In.NumFailed);

        if (In.FailedAsserts.empty())
        {
            return InOs;
        }

        for (const auto& [index, error] : std::views::enumerate(In.FailedAsserts))
        {
            const std::string lineInfo = error.Info.LineNumber == u32(-1) ? std::string{ "" } : std::format("Line: {}", error.Info.LineNumber);
            const std::string threadInfo = error.Info.ThreadIdType == 0 ? std::string{ "" } : ThreadInfoToString(static_cast<EThreadIdType>(error.Info.ThreadIdType), error.Info.ThreadId, In.DispatchDimensions);

            InOs << std::format("Assert {}: {} {}\n", index, lineInfo, threadInfo);
            u32 indentLevel = 0;
            bool validTree = true;
            auto tabbedWriter =
                [&InOs, &indentLevel](const auto InToWrite)
                {
                    for (u32 index = 0; index < indentLevel; index++)
                    {
                        InOs << "\t";
                    }

                    InOs << InToWrite;
                };

            if (error.Info.SectionId != -1 && error.Info.SectionId < In.Sections.size())
            {
                auto sectionPrinter =
                    [&indentLevel, &validTree, &tabbedWriter, &In](this auto&& InSelf, const i32 InSectionId)
                    {
                        if (InSectionId < -1 || InSectionId >= In.Sections.size())
                        {
                            indentLevel = 0;
                            validTree = false;
                            tabbedWriter("INVALID SECTION ID. Aborting Section reporting\n");
                            return;
                        }

                        const bool isSection = In.Sections[InSectionId].ParentId >= 0;
                        const i32 stringId = In.Sections[InSectionId].StringId;
                        const bool hasString = stringId >= 0 && stringId < In.Strings.size();

                        if (isSection)
                        {
                            InSelf(In.Sections[InSectionId].ParentId);
                        }

                        if (validTree)
                        {
                            tabbedWriter(std::format("{}: {}\n", isSection ? "SECTION"sv : "SCENARIO"sv, hasString ? In.Strings[stringId] : "UNKNOWN"s));
                        }

                        ++indentLevel;
                    };

                sectionPrinter(error.Info.SectionId);
            }

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

                tabbedWriter(std::format("Data {}: {}\n", i + 1u, error.ByteReader(error.TypeId, std::span{ error.Data.cbegin() + byteIndex, size })));
                byteIndex = AlignedOffset(byteIndex + size, 4);
            }
        }

        return InOs;
    }

    std::ostream& operator<<(std::ostream& InOs, const ErrorTypeAndDescription& In)
    {
        InOs << "Type: " << Enum::ScopedName(In.Type) << "\n Description: " << In.Error;
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
                [&InOs](const ErrorTypeAndDescription& InCompilationError)
                {
                    InOs << InCompilationError;
                }
            }, In.m_Result);

        return InOs;
    }
}