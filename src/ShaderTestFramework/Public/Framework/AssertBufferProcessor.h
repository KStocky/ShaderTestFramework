#pragma once

#include "Platform.h"

#include "Framework/HLSLTypes.h"
#include "Utility/Concepts.h"
#include "Utility/OverloadSet.h"
#include "Utility/Type.h"

#include <compare>
#include <functional>
#include <span>
#include <string>
#include <variant>
#include <vector>

namespace STF
{
    enum class EThreadIdType : u8
    {
        None,
        Int,
        Int3
    };

    struct AssertMetaData
    {
        u32 LineNumber = 0;
        u32 ThreadId = 0;
        u32 ThreadIdType = 0;

        friend auto operator<=>(const AssertMetaData&, const AssertMetaData&) = default;
    };

    struct HLSLAssertMetaData : AssertMetaData
    {
        u16 TypeId = 0;
        u16 ReaderId = 0;
        u32 DataAddress = 0;
        u32 DataSize = 0;

        friend auto operator<=>(const HLSLAssertMetaData&, const HLSLAssertMetaData&) = default;
    };

    struct AssertBufferLayout
    {
        u32 NumFailedAsserts = 0;
        u32 NumBytesAssertData = 0;

        friend auto operator<=>(const AssertBufferLayout&, const AssertBufferLayout&) = default;
    };

    using SingleTypeByteReader = std::function<std::string(std::span<const std::byte>)>;
    using MultiTypeByteReader = std::function<std::string(const u16, std::span<const std::byte>)>;

    std::string DefaultByteReader(const u16, std::span<const std::byte> InBytes);

    using MultiTypeByteReaderMap = std::vector<MultiTypeByteReader>;

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
        u32 NumSucceeded = 0;
        u32 NumFailed = 0;
        uint3 DispatchDimensions;

        friend auto operator<=>(const TestRunResults&, const TestRunResults&) = default;
        friend std::ostream& operator<<(std::ostream& InOs, const TestRunResults& In);
    };

    struct FailedShaderCompilationResult
    {
        std::string Error;
        friend auto operator<=>(const FailedShaderCompilationResult&, const FailedShaderCompilationResult&) = default;
        friend std::ostream& operator<<(std::ostream& InOs, const FailedShaderCompilationResult& In);
    };

    class Results
    {
    public:

        Results() = default;
        Results(FailedShaderCompilationResult InError);
        Results(TestRunResults InResults);

        template<typename ThisType>
        operator bool(this ThisType&& InThis)
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
            [](const FailedShaderCompilationResult&)
            {
                return false;
            } }, InThis.m_Result);
        }

        const TestRunResults* GetTestResults() const;
        const FailedShaderCompilationResult* GetFailedCompilationResult() const;

        friend std::ostream& operator<<(std::ostream& InOs, const Results& In);

    private:
        std::variant<std::monostate, TestRunResults, FailedShaderCompilationResult> m_Result;
    };

    TestRunResults ProcessAssertBuffer(
        const u32 InNumSuccessful,
        const u32 InNumFailed,
        const uint3 InDispatchDimensions,
        const AssertBufferLayout InLayout,
        std::span<const std::byte> InAssertData,
        const MultiTypeByteReaderMap& InByteReaderMap);

    template<HLSLTypeTriviallyConvertibleType T>
    SingleTypeByteReader CreateSingleTypeReader()
    {
        return
            [](const std::span<const std::byte> InBytes) -> std::string
            {
                if (InBytes.size_bytes() != sizeof(T))
                {
                    return std::format("Unexpected num bytes: {} for type {}", InBytes.size_bytes(), TypeToString<T>());
                }

                T data;
                std::memcpy(&data, InBytes.data(), sizeof(T));

                return std::format("{}", data);
            };
    }

    template<HLSLTypeTriviallyConvertibleType... T>
    MultiTypeByteReader CreateMultiTypeByteReader()
    {
        static constexpr auto numTypes = sizeof...(T);
        return
            [readers = std::array{ (CreateSingleTypeReader<T>())...}](const u16 InReaderId, const std::span<const std::byte> InBytes) -> std::string
            {
                if (InReaderId >= readers.size())
                {
                    return std::format("ReaderId must be less than the number of types used to construct this byte reader. ReaderId = {}, NumTypes = {}", InReaderId, numTypes);
                }

                return readers[InReaderId](InBytes);
            };
    }
}
