#pragma once

#include "Platform.h"
#include "Framework/AssertionsV1/TestDataBufferLayout.h"
#include "Framework/TypeByteReader.h"

#include "Utility/Error.h"
#include "Utility/HLSLTypes.h"

#include <compare>
#include <string>
#include <variant>
#include <vector>

namespace stf::AssertionsV1
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
        std::vector<std::byte> Data{};
        MultiTypeByteReader ByteReader{};
        AssertMetaData Info{};
        u16 TypeId = 0;

        friend bool operator==(const FailedAssert&, const FailedAssert&);
        friend bool operator!=(const FailedAssert&, const FailedAssert&);
    };

    struct TestRunResults
    {
        std::vector<FailedAssert> FailedAsserts{};
        std::vector<std::string> Strings{};
        std::vector<SectionInfoMetaData> Sections{};
        u32 NumSucceeded = 0;
        u32 NumFailed = 0;

        bool Succeeded() const { return NumFailed == 0; }

        friend bool operator==(const TestRunResults&, const TestRunResults&) = default;
        friend std::ostream& operator<<(std::ostream& InOs, const TestRunResults& In);
    };

    class Results
    {
    public:

        Results() = default;
        Results(Error InError);
        Results(TestRunResults InResults);

        operator bool() const;

        const TestRunResults* GetTestResults() const;
        const Error* GetTestRunError() const;

        friend std::ostream& operator<<(std::ostream& InOs, const Results& In);

    private:
        std::variant<std::monostate, TestRunResults, Error> m_Result;
    };
}