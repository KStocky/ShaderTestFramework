#pragma once

#include <tuplet/tuple.hpp>

namespace
{
    template<typename... InTypes>
    using Tuple = tuplet::tuple<InTypes...>;

    template <size_t I, tuplet::indexable Tup>
    constexpr decltype(auto) Get(Tup&& tup) {
        return tuplet::get<I>(std::forward<Tup>(tup));
    }
}