#pragma once

#include "Platform.h"

#include "Utility/StringLiteral.h"
#include <algorithm>
#include <string_view>

namespace stf
{
    template<u64 InSize>
    struct FixedString
    {
        char Data[InSize] = {};
        static constexpr auto Length = InSize - 1;

        constexpr FixedString(const char(&InString)[InSize])
        {
            std::copy(std::cbegin(InString), std::cend(InString), std::begin(Data));
        }

        constexpr std::string_view View() const
        {
            return std::string_view{ Data };
        }

        consteval StringLiteral Literal() const
        {
            return StringLiteral{ View() };
        }
    };

    template<u64 InLength>
    FixedString(const char(&)[InLength]) -> FixedString<InLength>;
}