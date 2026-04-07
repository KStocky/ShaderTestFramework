
#include "Platform.h"
#include "Utility/Concepts.h"
#include "Utility/Tuple.h"

namespace stf
{
    namespace detail
    {
        template<auto EnumVal, typename DefaultType, int Index, typename TupleType>
        struct EnumValToTypeFinder;

        template<auto EnumVal, typename DefaultType, int Index, typename... MappingTypes>
            requires CEnumType<decltype(EnumVal)>
        struct EnumValToTypeFinder<EnumVal, DefaultType, Index, Tuple<MappingTypes...>>
        {
            using type = decltype(
                []()
                {
                    if constexpr (Index < 0)
                    {
                        return std::type_identity<DefaultType>{};
                    }
                    else if constexpr (std::tuple_element_t<Index, Tuple<MappingTypes...>>::value == EnumVal)
                    {
                        return std::type_identity<typename std::tuple_element_t<Index, Tuple<MappingTypes...>>::type>{};
                    }
                    else
                    {
                        return std::type_identity<typename EnumValToTypeFinder<EnumVal, DefaultType, Index - 1, Tuple<MappingTypes...>>::type>{};
                    }
                }()
                    )::type;
        };
    }

    template<auto Val, typename T>
        requires CEnumType<decltype(Val)>
    struct EnumToType
    {
        using type = T;
        static constexpr auto value = Val;
    };

    template<CEnumType EnumType, typename MappingType>
    struct EnumValsToTypes;

    template<CEnumType EnumType, EnumType... EnumVals, typename... Ts>
    struct EnumValsToTypes<EnumType, Tuple<EnumToType<EnumVals, Ts>...>>
    {
        using TupleType = Tuple<EnumToType<EnumVals, Ts>...>;
        template<EnumType ValToFind, typename DefaultType>
        using FindTypeOr = typename detail::EnumValToTypeFinder<ValToFind, DefaultType, std::tuple_size_v<TupleType> -1, TupleType>::type;
    };
}