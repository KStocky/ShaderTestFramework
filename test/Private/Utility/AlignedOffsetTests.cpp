#include <Utility/Math.h>

namespace AlignedOffsetTests
{
    static_assert(4 == stf::AlignedOffset(4, 4));
    static_assert(0 == stf::AlignedOffset(0, 4));
    static_assert(8 == stf::AlignedOffset(5, 4));
    static_assert(4 == stf::AlignedOffset(3, 4));
}