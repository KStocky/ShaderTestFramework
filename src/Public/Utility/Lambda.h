#pragma once

#include "Utility/Concepts.h"
#include "Utility/FunctionTraits.h"
#include "Utility/Tuple.h"
#include "Utility/TypeList.h"

namespace stf
{

    template<auto InFunc, typename InCaptureTypes, typename InCaptureIndices>
    class Lambda;

    template<auto InFunc, typename... InCaptureTypes, size_t... InCaptureIndices>
    class Lambda<InFunc, TypeList<InCaptureTypes...>, std::index_sequence<InCaptureIndices...>>
    {
        using RawFuncTraits = TFuncTraits<decltype(InFunc), TypeList<InCaptureTypes...>>;

        template<typename T>
        struct ConstCaptureTransformer
        {
            using Type = std::conditional_t<RawFuncTraits::IsConst, const T, T>;
        };

        using UnsortedCaptureTypes = typename TypeList<InCaptureTypes...>::template Transform<ConstCaptureTransformer>;

        template<typename InSortedIndices, typename InUnsortedIndices, typename InUnsortedTypes>
        struct Sorter;

        template<size_t... InSortedIndices, size_t... InUnsortedIndices, typename... InUnsortedTypes>
        struct Sorter<std::index_sequence<InSortedIndices...>, std::index_sequence<InUnsortedIndices...>, TypeList<InUnsortedTypes...>>
        {
            using CaptureType = Tuple<typename TypeList<InUnsortedTypes...>::template Type<InSortedIndices>...>;
            using UnsortIndices = std::index_sequence<InUnsortedIndices...>;
            using SortIndices = std::index_sequence<InSortedIndices...>;

            template<size_t InIndex, typename T, typename... InOtherTypes>
                requires (InIndex == 0)
            static constexpr decltype(auto) NthArg(T&& In, InOtherTypes&&...)
            {
                return std::forward<T>(In);
            }

            template<size_t InIndex, typename T, typename... InOtherTypes>
                requires (InIndex > 0)
            static constexpr decltype(auto) NthArg(T&&, InOtherTypes&&... InOthers)
            {
                return NthArg<InIndex - 1>(std::forward<InOtherTypes>(InOthers)...);
            }

            template<size_t InIndex, size_t InMapIndex, size_t... InOtherMapIndices>
                requires (InIndex == 0)
            static constexpr size_t NthIndex(std::index_sequence<InMapIndex, InOtherMapIndices...>)
            {
                return InMapIndex;
            }

            template<size_t InIndex, size_t InMapIndex, size_t... InOtherMapIndices>
                requires (InIndex > 0)
            static constexpr size_t NthIndex(std::index_sequence<InMapIndex, InOtherMapIndices...>)
            {
                return NthIndex<InIndex - 1>(std::index_sequence<InOtherMapIndices...>{});
            }

            template<size_t InIndex, size_t... InMappedIndices, typename... InTypes>
            static constexpr decltype(auto) SwizzleVariadic(std::index_sequence<InMappedIndices...> InIndices, InTypes&&... InArgs)
            {
                constexpr auto mappedIndex = NthIndex<InIndex>(InIndices);
                return NthArg<mappedIndex>(std::forward<InTypes>(InArgs)...);
            }
        };

        static constexpr auto TypeToKey = []<typename T>() { return alignof(T); };
        static constexpr auto KeyCompare = [](size_t InA, size_t InB) { return InA > InB; };

        using CaptureSorter = Sorter
            <
            typename UnsortedCaptureTypes::template SortedIndices<TypeToKey, KeyCompare>,
            typename UnsortedCaptureTypes::template UnsortedIndices<TypeToKey, KeyCompare>,
            UnsortedCaptureTypes
            >;

        using SortedTuple = typename CaptureSorter::CaptureType;

        using FuncTraits = TFuncTraits<decltype(InFunc), UnsortedCaptureTypes>;

    public:

        using FuncType = FuncTraits::CallSignature;
        using CaptureTypes = UnsortedCaptureTypes;
        using ParamTypes = FuncTraits::ParamTypes::template RemoveRange<0, CaptureTypes::Size>;
        using RetType = FuncTraits::ReturnType;
        using CaptureVarFuncParamTypes = FuncTraits::ParamTypes::template RemoveRange<CaptureTypes::Size, ParamTypes::Size>;
        static constexpr bool IsConstCall = FuncTraits::IsConst;

        struct StaticAssertConditions
        {
            static constexpr bool CaptureVarsArePassedByRef = CaptureVarFuncParamTypes::AllOf([]<typename T>() { return std::is_reference_v<T>; });
            static constexpr bool CaptureByValVarsArePassedByConstRefForConstCall = !IsConstCall || !CaptureTypes::AnyOf(
                []<typename T, typename U>()
            {
                const bool CaptureIsByVal = !std::is_pointer_v<T>;
                const bool PassedByNonConstRef = !std::is_const_v<std::remove_reference_t<U>>;
                return CaptureIsByVal && PassedByNonConstRef;
            }, CaptureVarFuncParamTypes{});
        };

        template<typename InFuncType, typename... InOtherCaptureTypes>
        constexpr Lambda(InFuncType, InOtherCaptureTypes&&... InCaptures)
            : m_Captures{ CaptureSorter::template SwizzleVariadic<InCaptureIndices>(CaptureSorter::SortIndices(), std::forward<InOtherCaptureTypes>(InCaptures)...)... }
        {
            static_assert(StaticAssertConditions::CaptureVarsArePassedByRef, "The parameters that you are using to pass your captured variables to the function are not all references. Please ensure that you are passing your captures as either a const reference or a reference");
            static_assert(StaticAssertConditions::CaptureByValVarsArePassedByConstRefForConstCall, "You are passing a capture by value arg by non-const reference when the function call is const. Either pass by const reference, or mark the function as mutable ");
        }

        template<typename... InOtherCallParamTypes>
        constexpr RetType operator()(InOtherCallParamTypes&&... InParams) noexcept(noexcept(Apply(CaptureSorter::UnsortIndices(), *this, std::forward<InOtherCallParamTypes>(InParams)...))) requires (!IsConstCall)
        {
            return Apply(CaptureSorter::UnsortIndices(), *this, std::forward<InOtherCallParamTypes>(InParams)...);
        }

        template<typename... InOtherCallParamTypes>
        constexpr RetType operator()(InOtherCallParamTypes&&... InParams) const noexcept(noexcept(Apply(CaptureSorter::UnsortIndices(), *this, std::forward<InOtherCallParamTypes>(InParams)...))) requires (IsConstCall)
        {
            return Apply(CaptureSorter::UnsortIndices(), *this, std::forward<InOtherCallParamTypes>(InParams)...);
        }

    private:

        template<typename T>
        static constexpr bool DerefReq = std::is_pointer_v<std::remove_reference_t<T>>;

        template<typename T>
            requires !DerefReq<T>
        static constexpr decltype(auto) Deref(T&& In) noexcept
        {
            return In;
        }

        template<typename T>
            requires DerefReq<T>
        static constexpr decltype(auto) Deref(T In) noexcept
        {
            return *In;
        }

        template<typename ThisType, size_t... InIndices, typename... InOtherCallParamTypes>
        static constexpr RetType Apply(std::index_sequence<InIndices...>, ThisType&& InThis, InOtherCallParamTypes&&... InParams) noexcept(noexcept(std::invoke(const_cast<std::remove_const_t<decltype(InFunc)>&>(InFunc), Deref(get<InIndices>(std::forward<ThisType>(InThis).m_Captures))..., std::forward<InOtherCallParamTypes>(InParams)...)))
        {
            return std::invoke(const_cast<std::remove_const_t<decltype(InFunc)>&>(InFunc), Deref(get<InIndices>(std::forward<ThisType>(InThis).m_Captures))..., std::forward<InOtherCallParamTypes>(InParams)...);
        }

        STF_NO_UNIQUE_ADDRESS
        SortedTuple m_Captures;
    };

    template<typename InLambdaType, typename... InCaptureTypes>
        requires ConstexprDefaultConstructableEmptyCallableType<InLambdaType, InCaptureTypes...>
    Lambda(InLambdaType, InCaptureTypes...)->Lambda < InLambdaType{}, TypeList<InCaptureTypes...>, std::index_sequence_for<InCaptureTypes... >> ;

    template<typename T>
    struct TIsLambdaInstantiation : std::false_type {};

    template<auto InFunc, typename... InArgTypes>
    struct TIsLambdaInstantiation<Lambda<InFunc, InArgTypes...>> : std::true_type {};

    template<typename T>
    concept LambdaType = TIsLambdaInstantiation<T>::value;

}