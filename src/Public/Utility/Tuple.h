#pragma once
#include <tuplet/tuple.hpp>

namespace stf
{
    template<typename... InTypes>
    using Tuple = tuplet::tuple<InTypes...>;

    template <class... T>
    constexpr Tuple<T&...> tie(T&... t) {
        return { t... };
    }
}