#pragma once
#include <string_view>

namespace stf
{
    namespace TypePrivate
    {
        constexpr std::string_view TypeBeginToken =
#ifdef __clang__
            "= ";
#else
            "TypeToString<";
#endif

        constexpr std::string_view TypeEndToken =
#ifdef __clang__
            "]";
#else
            ">";
#endif

    }

    template<typename T>
    constexpr std::string_view TypeToString()
    {
        constexpr std::string_view funcName = __FUNCSIG__;

        const auto endIndex = funcName.find_last_of(TypePrivate::TypeEndToken);
        const auto startIndex = funcName.rfind(TypePrivate::TypeBeginToken) + TypePrivate::TypeBeginToken.size();

        const auto ret = funcName.substr(startIndex, (endIndex - startIndex));
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