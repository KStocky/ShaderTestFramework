#pragma once

#include <string>
#include <string_view>

namespace stf
{
    struct TransparentStringHash
    {
        using hash_type = std::hash<std::string_view>;
        using is_transparent = void;

        std::size_t operator()(const char* str) const { return hash_type{}(str); }
        std::size_t operator()(std::string_view str) const { return hash_type{}(str); }
        std::size_t operator()(std::string const& str) const { return hash_type{}(str); }
    };
}