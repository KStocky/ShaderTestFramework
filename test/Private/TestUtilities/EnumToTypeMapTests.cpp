
#include "TestUtilities/EnumToTypeMap.h"

namespace stf::EnumToTypeMapTests
{
    enum class EVals
    {
        One,
        Two,
        Three,
        Four
    };

    struct A {};
    struct B {};
    struct C {};

    struct MissingType {};

    using Mapping =
        EnumValsToTypes<
            EVals,
            Tuple<
                EnumToType<EVals::One, A>,
                EnumToType<EVals::Two, B>,
                EnumToType<EVals::Three, C>
            >
        >;

    static_assert(std::same_as<B, Mapping::template FindTypeOr<EVals::Two, MissingType>>);
    static_assert(std::same_as<MissingType, Mapping::template FindTypeOr<EVals::Four, MissingType>>);
    static_assert(std::same_as<A, Mapping::template FindTypeOr<EVals::One, MissingType>>);
    static_assert(std::same_as<C, Mapping::template FindTypeOr<EVals::Three, MissingType>>);
}