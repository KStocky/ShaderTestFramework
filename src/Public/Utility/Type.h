#pragma once
#include <string_view>

namespace stf
{
    template<typename T>
    constexpr std::string_view TypeToString()
    {
        constexpr std::string_view funcName = __FUNCSIG__;

        const auto startIndex = funcName.find_last_of('<');
        const auto endIndex = funcName.find_last_of('>');
        const auto ret = funcName.substr(startIndex + 1, (endIndex - startIndex) - 1);
        return ret;
    }


    template<typename... InTypes>
    constexpr bool AlwaysFalse = false;

    template<typename... T>
    void CheckTemplateArgs()
    {
        static_assert(AlwaysFalse<T...>);
    }
}