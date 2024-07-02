#pragma once

#include "float16/float16_t.hpp"
#include <format>
#include <sstream>

using float16_t = numeric::float16_t;

template <>
struct std::formatter<float16_t> : std::formatter<string_view> {
    auto format(const float16_t In, auto& ctx) const {
        std::ostringstream temp;
        temp << In;
        return std::formatter<string_view>::format(temp.str(), ctx);
    }
};

