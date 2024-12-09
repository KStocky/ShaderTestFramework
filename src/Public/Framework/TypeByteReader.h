#pragma once

#include "Platform.h"

#include "Utility/Concepts.h"
#include "Utility/Type.h"

#include <cstddef>
#include <format>
#include <functional>
#include <span>
#include <string>
#include <vector>

namespace stf
{
    using SingleTypeByteReader = std::function<std::string(std::span<const std::byte>)>;
    using MultiTypeByteReader = std::function<std::string(const u16, std::span<const std::byte>)>;

    std::string DefaultByteReader(const u16, std::span<const std::byte> InBytes);

    using MultiTypeByteReaderMap = std::vector<MultiTypeByteReader>;

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
            [readers = std::array{ (CreateSingleTypeReader<T>())... }](const u16 InReaderId, const std::span<const std::byte> InBytes) -> std::string
            {
                if (InReaderId >= readers.size())
                {
                    return std::format("ReaderId must be less than the number of types used to construct this byte reader. ReaderId = {}, NumTypes = {}", InReaderId, numTypes);
                }

                return readers[InReaderId](InBytes);
            };
    }
}