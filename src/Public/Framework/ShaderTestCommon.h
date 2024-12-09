#pragma once

#include "Platform.h"

#include "Framework/HLSLTypes.h"
#include "Framework/TypeByteReader.h"
#include "Framework/TestDataBufferLayout.h"

#include <compare>
#include <string>
#include <variant>
#include <vector>

namespace stf
{
    enum class EThreadIdType : u8
    {
        None,
        Int,
        Int3
    };

    struct AllocationBufferData
    {
        u32 NumPassedAsserts = 0;
        u32 NumFailedAsserts = 0;
        u32 NumBytesAssertData = 0;
        u32 NumStrings = 0;
        u32 NumBytesStringData = 0;
        u32 NumSections = 0;
    };

    struct FailedAssert
    {
        std::vector<std::byte> Data;
        MultiTypeByteReader ByteReader;
        AssertMetaData Info;
        u16 TypeId = 0;

        friend bool operator==(const FailedAssert&, const FailedAssert&);
        friend bool operator!=(const FailedAssert&, const FailedAssert&);
    };

    struct TestRunResults
    {
        std::vector<FailedAssert> FailedAsserts;
        std::vector<std::string> Strings;
        std::vector<SectionInfoMetaData> Sections;
        u32 NumSucceeded = 0;
        u32 NumFailed = 0;
        uint3 DispatchDimensions;

        friend auto operator<=>(const TestRunResults&, const TestRunResults&) = default;
        friend std::ostream& operator<<(std::ostream& InOs, const TestRunResults& In);
    };

    enum class ETestRunErrorType
    {
        Unknown,
        ShaderCompilation,
        Binding,
        RootSignatureGeneration
    };

    struct ErrorTypeAndDescription
    {
        ETestRunErrorType Type;
        std::string Error;
        friend auto operator<=>(const ErrorTypeAndDescription&, const ErrorTypeAndDescription&) = default;
        friend std::ostream& operator<<(std::ostream& InOs, const ErrorTypeAndDescription& In);
    };

    class Results
    {
    public:

        Results() = default;
        Results(ErrorTypeAndDescription InError);
        Results(TestRunResults InResults);

        operator bool() const;

        const TestRunResults* GetTestResults() const;
        const ErrorTypeAndDescription* GetTestRunError() const;

        friend std::ostream& operator<<(std::ostream& InOs, const Results& In);

    private:
        std::variant<std::monostate, TestRunResults, ErrorTypeAndDescription> m_Result;
    };
}