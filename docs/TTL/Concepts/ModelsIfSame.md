# ttl::models_if_same

A utility function to be used within the definition of a [pseudo concept](./PseudoConcepts.md). `ttl::models_if_same` is similar to [`ttl::is_same`](../TypeTraits/IsSame.md) in that it compares two types. If the two types, `T` and `U`, are the same, this function will be well defined, otherwise this function will be ill-formed.

Note: `ttl::models_if_same` is not defined, so it only allowed to be used in an unevaluated context such as within a `__decltype` expression.

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

template<typename T, typename U>
typename enable_if<is_same<T, U>::value>::type models_if_same(); 

```

## Template Parameters

1. `T` -> First type to compare.
2. `U` -> Second type to compare.

## Parameters

None

## Return value

Cannot be called and thus never returns a value. The return type is of type `void` if `T` and `U` are the same type. Otherwise, the return type is not defined.

## Example

```c++

#include "/Test/TTL/models.hlsli"

struct FooReturnsInt
{
    template<typename T>
    __decltype(
        ttl::models_if_same<
            int, __decltype(ttl::declval<T>().Foo())
        >()
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
_Static_assert(ttl::models<FooReturnsInt, A>::value, "Returns false because B's Foo member function does not return an int");

```
---

[Top](#ttlmodels_if_same)