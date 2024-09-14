#pragma once
#include <type_traits>

namespace stf
{
    namespace PrivateUnqualified
    {
        template<typename T>
        struct TUnqualified
        {
            using Type = T;
        };

        template<typename T>
        struct TUnqualified<T*>
        {
            using Type = TUnqualified<T>::Type;
        };

        template<typename T>
        struct TUnqualified<const T>
        {
            using Type = TUnqualified<T>::Type;
        };

        template<typename T>
        struct TUnqualified<T&>
        {
            using Type = TUnqualified<T>::Type;
        };

        template<typename T>
        struct TUnqualified<T* const>
        {
            using Type = TUnqualified<T>::Type;
        };

        template<typename T>
        struct TUnqualified<T&&>
        {
            using Type = TUnqualified<T>::Type;
        };

        template<typename RetType, typename... InArgs>
        struct TUnqualified<RetType(*)(InArgs...)>
        {
            using Type = RetType(InArgs...);
        };
    }

    template<typename T>
    using TUnqualified = PrivateUnqualified::TUnqualified<T>::Type;

    template<typename T>
    using Unqualified = PrivateUnqualified::TUnqualified<T>;


    // This template check won't work for any template that takes a NTTP
    // This paper talks about what needs to be in the standard for this to occur.
    // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1985r3.pdf
    template<template<typename...> class Template, typename T>
    struct TIsInstantiationOf
    {
        static constexpr bool Value = false;
    };

    template<template<typename...> class Template, typename... InArgs>
    struct TIsInstantiationOf<Template, Template<InArgs...>>
    {
        static constexpr bool Value = true;
    };
}