#pragma once

#include "Utility/Concepts.h"
#include "Utility/Type.h"

#include <expected>
#include <ostream>

namespace stf
{
    template<typename T, typename E>
    using Expected = std::expected<T, E>;

    template<typename E>
    using Unexpected = std::unexpected<E>;
}

template<typename T, typename E>
std::ostream& operator<<(std::ostream& InOutStream, const stf::Expected<T, E>& InExpected)
{
    if (InExpected.has_value())
    {
        if constexpr (stf::OStreamable<T>)
        {
            InOutStream << InExpected.value();
        }
        else
        {
            std::print(InOutStream, "Unstreamable expected type: {}", stf::TypeToString<T>());
        }
    }
    else
    {
        if constexpr (stf::OStreamable<E>)
        {
            InOutStream << InExpected.error();
        }
        else
        {
            std::print(InOutStream, "Unstreamable error type: {}", stf::TypeToString<E>());
        }
    }

    return InOutStream;
}
