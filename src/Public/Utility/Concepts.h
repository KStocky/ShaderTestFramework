#pragma once

#include "Platform.h"
#include "TypeTraits.h"
#include <concepts>
#include <format>
#include <type_traits>

namespace stf
{
    namespace ConceptsPrivate
    {
        template<auto T>
        struct TestLiteralType {};
    }

    template<typename T, typename... Us>
    concept IsAnyOf = (std::same_as<T, Us> || ...);

    template<typename T>
    concept EmptyType = std::is_empty_v<T>;

    template<typename T>
    concept FinalType = std::is_final_v<T>;

    template<typename T>
    concept PointerType = std::is_pointer_v<T>;

    template<typename T>
    concept DefaultConstructibleType = std::is_default_constructible_v<T>;

    template<typename T>
    concept MoveAssignableType = std::is_move_assignable_v<T>;

    template<typename T>
    concept MoveConstructibleType = std::is_move_constructible_v<T>;

    template<typename T>
    concept PureFunctionType = std::is_function_v<std::remove_pointer_t<std::remove_reference_t<T>>>;

    template<typename T>
    concept CapturelessLambdaType = !PureFunctionType<T> && requires(T InT)
    {
        { +InT } -> PureFunctionType;
    };

    template<typename T>
    concept NonTemplatedCallableType = requires()
    {
        typename std::void_t<decltype(&std::decay_t<T>::operator())>;
    };

    template<typename T>
    concept ConstexprDefaultConstructableType = requires()
    {
        typename ConceptsPrivate::TestLiteralType < T{} > ;
    };

    template<typename T, typename... U>
    concept TemplatedCallableType = requires()
    {
        typename std::void_t<decltype(&std::decay_t<T>::template operator() < U... > )>;
    };

    template<typename T, typename... U>
    concept CallableType = (TemplatedCallableType<T, U...> || NonTemplatedCallableType<T>);

    template<typename T, typename... U>
    concept EmptyCallableType = std::is_empty_v<T> && CallableType<T, U...>;

    template<typename T, typename... U>
    concept ConstexprDefaultConstructableEmptyCallableType = ConstexprDefaultConstructableType<T> && EmptyCallableType<T, U...>;

    template<template<typename...> typename Template, typename... Ts>
    concept InstantiatableFrom = TIsInstantiationOf<Template, Template<Ts...>>::Value;

    template<typename T, typename... Ts>
    concept Newable = requires(void* InBuff, Ts&&... In)
    {
        { new T(std::forward<Ts>(In)...) };
        { new (InBuff) T(std::forward<Ts>(In)...) };
    };

    template<typename T>
    concept HLSLBaseType =
        std::same_as<T, bool> ||
        std::same_as<T, i16> ||
        std::same_as<T, i32> ||
        std::same_as<T, i64> ||
        std::same_as<T, u16> ||
        std::same_as<T, u32> ||
        std::same_as<T, u64> ||
        std::same_as<T, f16> ||
        std::same_as<T, f32> ||
        std::same_as<T, f64>;

    namespace Private
    {
        template <class Ty, class Context, class Formatter = Context::template formatter_type<std::remove_const_t<Ty>>>
        concept Formattable_with = std::semiregular<Formatter>
            && requires(Formatter & f, const Formatter & cf, Ty && t, Context fc,
                std::basic_format_parse_context<typename Context::char_type> pc) {
                    { f.parse(pc) } -> std::same_as<typename decltype(pc)::iterator>;
                    { cf.format(t, fc) } -> std::same_as<typename Context::iterator>;
        };
        template <class CharT>
        struct FakeFormatIter {

            using iterator_category = std::output_iterator_tag;
            using value_type = CharT;
            using difference_type = ptrdiff_t;
            using pointer = CharT*;
            using reference = CharT&;

            reference operator*() const;
            pointer operator->() const;

            FakeFormatIter& operator++();
            FakeFormatIter operator++(int);
        };

        template <class Ty, class CharT>
        concept formattable =
            Formattable_with<std::remove_reference_t<Ty>, std::basic_format_context<FakeFormatIter<CharT>, CharT>>;

    }

    template<typename T, typename CharT>
    concept Formattable =
#if _MSC_VER > 1938
        std::formattable<T, CharT>;
#else
        Private::formattable<T, CharT>;
#endif

    template<typename T>
    concept HLSLTypeTriviallyConvertibleType =
        std::is_trivially_copyable_v<T> &&
        (alignof(T) == 4 || alignof(T) == 2 || alignof(T) == 8) &&
        Formattable<T, char>;

}
