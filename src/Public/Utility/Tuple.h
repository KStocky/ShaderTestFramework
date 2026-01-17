#pragma once
#include <tuplet/tuple.hpp>
#include <tuple>

namespace stf
{
#if defined(__clang__) && __clang_major__ <= 19
    template<typename... InTypes>
    using Tuple = std::tuple<InTypes...>;
#else
    template<typename... InTypes>
    using Tuple = tuplet::tuple<InTypes...>;
#endif
}