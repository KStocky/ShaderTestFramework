
#include "TestUtilities/UniqueType.h"

#include <concepts>

namespace stf::UniqueTypeTests
{
    struct Empty {};

    using Type1 = UniqueType<Empty>;

    static_assert(std::same_as<Type1, Type1>);
    static_assert(!std::same_as<UniqueType<Empty>, UniqueType<Empty>>);
    static_assert(!std::convertible_to<Empty, Type1>);
    static_assert(std::convertible_to<Type1, Empty>);
}