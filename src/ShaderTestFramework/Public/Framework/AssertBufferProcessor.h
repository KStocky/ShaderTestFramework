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

    struct HLSLAssertMetaData
    {
        u32 LineNumber = 0;
        u32 ThreadId = 0;
        u32 ThreadIdType = 0;
        u32 TypeId = 0;
        u32 DataAddress = 0;
        u32 DataSize = 0;

        auto operator<=>(const HLSLAssertMetaData& InB) const = default;
    };

    struct AssertBufferLayout
    {
        u32 NumFailedAsserts = 0;
        u32 NumBytesAssertData = 0;

        auto operator<=>(const AssertBufferLayout& InB) const = default;
    };

    using TypeConverter = std::function<std::string(std::span<const std::byte>)>;

    using TypeConverterMap = std::vector<TypeConverter>;

    struct FailedAssert
    {
        std::vector<std::byte> Data;
        HLSLAssertMetaData Info;
    };

    struct TestRunResults
    {
        std::vector<FailedAssert> FailedAsserts;
        u32 NumSucceeded = 0;
        u32 NumFailed = 0;
        uint3 DispatchDimensions;
    };

    class Results
    {
    public:

        Results() = default;
        Results(std::string InError);
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
            [](const std::string&)
            {
                return false;
            } }, InThis.m_Result);
        }

        friend std::ostream& operator<<(std::ostream& InOs, const Results& In);

    private:
        std::variant<std::monostate, TestRunResults, std::string> m_Result;
    };

    TestRunResults ProcessAssertBuffer(
        const u32 InNumSuccessful,
        const u32 InNumFailed,
        const uint3 InDispatchDimensions,
        const AssertBufferLayout InLayout,
        std::span<const std::byte> InAssertData,
        const TypeConverterMap& InTypeHandlerMap);

    template<HLSLTypeTriviallyConvertibleType T>
    TypeConverter CreateDefaultTypeConverter()
    {
        return 
            [](const std::span<const std::byte> InBytes) -> std::string
            {
                if (InBytes.size_bytes() != sizeof(T))
                {
                    return std::format("Unexpected num bytes: {} for type {}", InBytes.size_bytes(), TypeToString<T>());
                }

                T data;
                std::memcpy(&data, InBytes.data(), InBytes.size_bytes());

                return std::format("{}", data);
            };
    }
}
