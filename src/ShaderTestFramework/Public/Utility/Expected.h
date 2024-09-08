#pragma once

#if _MSC_VER > 1936
#include <expected>

namespace stf
{
    template<typename T, typename E>
    using Expected = std::expected<T, E>;

    template<typename E>
    using Unexpected = std::unexpected<E>;
}
#else
#include <tl/expected.hpp>

namespace stf
{
    template<typename T, typename E>
    using Expected = tl::expected<T, E>;

    template<typename E>
    using Unexpected = tl::unexpected<E>;
}
#endif