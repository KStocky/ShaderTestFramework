#pragma once
#include "Platform.h"

constexpr inline u64 AlignedOffset(const u64 InOffset, const u64 InAlignment) noexcept
{
    const u64 AlignmentMinusOne = InAlignment - 1;
    return (InOffset + AlignmentMinusOne) & ~AlignmentMinusOne;
}