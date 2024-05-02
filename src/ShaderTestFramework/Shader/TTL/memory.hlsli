#pragma once

namespace ttl
{
    uint aligned_offset(const uint InOffset, const uint InAlignment)
    {
        return (InOffset - 1 + InAlignment) & -InAlignment;
    }

    template<typename T>
    void zero(inout T In)
    {
        In = (T)0;
    }

    template<typename T>
    T zero()
    {
        return (T)0;
    }
}