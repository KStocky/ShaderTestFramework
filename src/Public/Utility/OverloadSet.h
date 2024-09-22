#pragma once

namespace stf
{
    template<class... Ts>
    struct OverloadSet : Ts...
    {
        using Ts::operator()...;
    };
}