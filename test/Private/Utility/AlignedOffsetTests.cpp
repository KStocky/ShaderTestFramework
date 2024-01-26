#include <Utility/Math.h>

namespace AlignedOffsetTests
{
    static_assert(4 == AlignedOffset(4, 4));
    static_assert(0 == AlignedOffset(0, 4));
    static_assert(8 == AlignedOffset(5, 4));
    static_assert(4 == AlignedOffset(3, 4));
}