#pragma once
#include <tuple>
#include <tuplet/tuple.hpp>

namespace stf
{
    // Necessary due to https://developercommunity.visualstudio.com/t/Unexpected-behaviour-with-msvc::no_uniqu/10798165
    // TODO: When this is fixed, we should remove it and just use tuplet::tuple.
    template<typename... InTypes>
    using Tuple =
#if _MSC_VER >= 1942
        std::tuple<InTypes...>;
#else
        tuplet::tuple<InTypes...>;
#endif

    template <size_t I, typename Tup>
    constexpr decltype(auto) Get(Tup&& tup) {
        return get<I>(std::forward<Tup>(tup));
    }
}