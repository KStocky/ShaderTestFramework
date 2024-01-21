#pragma once

namespace ttl
{
    uint aligned_offset(const uint InOffset, const uint InAlignment)
    {
        return (InOffset - 1 + InAlignment) & -InAlignment;
    }
}