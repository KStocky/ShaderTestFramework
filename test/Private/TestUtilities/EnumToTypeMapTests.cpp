
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

    enum class EOtherVals
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
            EnumToType<EVals::One, A>,
            EnumToType<EVals::Two, B>,
            EnumToType<EVals::Three, C>
        >;

    static_assert(std::same_as<B, Mapping::template FindTypeOr<EVals::Two, MissingType>>);
    static_assert(std::same_as<MissingType, Mapping::template FindTypeOr<EVals::Four, MissingType>>);
    static_assert(std::same_as<A, Mapping::template FindTypeOr<EVals::One, MissingType>>);
    static_assert(std::same_as<C, Mapping::template FindTypeOr<EVals::Three, MissingType>>);

    template<typename... Ts>
    concept ValidateEnumValsToTypes = requires
    {
        typename EnumValsToTypes<Ts...>;
    };

    static_assert(!ValidateEnumValsToTypes<>);
    static_assert(!ValidateEnumValsToTypes<A>);
    static_assert(!ValidateEnumValsToTypes<A, B>);
    static_assert(!ValidateEnumValsToTypes<EnumToType<EVals::One, B>, EnumToType<EOtherVals::One, A>>);


    static_assert(ValidateEnumValsToTypes<EnumToType<EVals::One, A>>);
    static_assert(ValidateEnumValsToTypes<EnumToType<EVals::One, B>>);
    static_assert(ValidateEnumValsToTypes<EnumToType<EVals::One, B>, EnumToType<EVals::One, A>>);

}