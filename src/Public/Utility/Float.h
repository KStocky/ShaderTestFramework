#pragma once

#include "float16/float16_t.hpp"
#include <format>
#include <sstream>

namespace stf
{
    using float16_t = numeric::float16_t;
}

template <>
struct std::formatter<stf::float16_t> : std::formatter<string_view> {
    auto format(const stf::float16_t In, auto& ctx) const {
        std::ostringstream temp;
        temp << In;
        return std::formatter<string_view>::format(temp.str(), ctx);
    }
};

