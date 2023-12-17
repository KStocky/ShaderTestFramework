#pragma once

#include "Platform.h"
#include "Utility/Concepts.h"
#include "Utility/Type.h"

#include <functional>
#include <span>
#include <string>
#include <vector>

namespace STF
{
    struct HLSLAssertMetaData
    {
        u32 LineNumber = 0;
        u32 ThreadId = 0;
        u32 ThreadIdType = 0;
        u32 TypeId = 0;
        u32 DataAddress = 0;
        u32 DataSize = 0;
    };

    struct AssertBufferLayout
    {
        u32 NumFailedAsserts = 0;
        u32 NumBytesAssertData = 0;
    };

    using TypeConverter = std::function<std::string(std::span<const std::byte>)>;

    using TypeConverterMap = std::vector<TypeConverter>;

    std::vector<std::string> ProcessAssertBuffer(
        const u32 InNumSuccessful,
        const u32 InNumFailed,
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