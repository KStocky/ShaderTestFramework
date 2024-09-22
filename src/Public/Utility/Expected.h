#pragma once

#include <expected>

namespace stf
{
    template<typename T, typename E>
    using Expected = std::expected<T, E>;

    template<typename E>
    using Unexpected = std::unexpected<E>;
}
