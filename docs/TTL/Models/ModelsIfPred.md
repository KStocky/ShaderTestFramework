# ttl::models_if_pred

A utility function to be used within the definition of a [pseudo concept](./PseudoConcepts.md). `ttl::models_if_pred` takes template template argument which takes a single template argument. `ttl::models_if_pred` succeeds if the template template argument provides a `value` member constant that evaluates to `true`. The function is ill-formed otherwise. It is a generalization of [`ttl::models_if_same`](./ModelsIfSame.md).

Note: `ttl::models_if_pred` is not defined, so it only allowed to be used in an unevaluated context such as within a `__decltype` expression.

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Template Parameters](#template-parameters)
4. [Parameters](#parameters)
5. [Return Value](#return-value)
6. [Example](#example)

## Header

`/Test/TTL/models.hlsli`

## Declaration

```c++

template<template<typename> class T, typename U>
typename enable_if <T<U>::value>::type models_if_pred();

```

## Template Parameters

1. `T` -> Template template argument that takes a single template argument. Expected to have a `static const bool value` member on instantiation
2. `U` -> Type to supply to the template template argument, `T`.

## Parameters

None

## Return value

Cannot be called and thus never returns a value. The return type is of type `void` if `T<U>::value` evaluates to `true`. Otherwise, the return type is not defined.

## Example

```c++

#include "/Test/TTL/models.hlsli"

struct FooReturnsInt
{
    template<typename T>
    using is_int = ttl::is_same<int, T>;

    template<typename T>
    __decltype(
        ttl::models_if_pred<is_int, __decltype(ttl::declval<T>().Foo())>()
    ) requires();
};


struct A
{
    int Foo();
};

struct B
{
    void Foo();
};

_Static_assert(ttl::models<FooReturnsInt, A>::value, "Returns true because A's Foo member function returns an int");
_Static_assert(!ttl::models<FooReturnsInt, B>::value, "Returns false because B's Foo member function does not return an int");

```
---

[Top](#ttlmodels_if_pred)