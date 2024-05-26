[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Models](./ModelsHeader.md)

# ttl::models_if

A utility function to be used within the definition of a [pseudo concept](./PseudoConcepts.md). `ttl::models_if` will evaluate a compile time `bool` expression, such as a type trait. It will succeed if the expression evaluates to `true`.

Note: `ttl::models_if` is not defined, so it only allowed to be used in an unevaluated context such as within a `__decltype` expression.

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

template<bool Cond>
typename enable_if<Cond>::type models_if();

```

## Template Parameters

1. `Cond` -> Boolean expression.

## Parameters

None

## Return value

Cannot be called and thus never returns a value. The return type is of type `void` if the supplied `bool` argument is `true`. Otherwise, the return type is not defined.

## Example

```c++

#include "/Test/TTL/models.hlsli"

struct IsFourBytes
{
    template<typename T>
    __decltype(
        ttl::models_if<ttl::size_of<T>::value == 4>()
    ) requires();
};


struct A
{
    int data;
};

struct B
{
    int data1;
    int data2;
};

_Static_assert(ttl::models<IsFourBytes, A>::value, "Returns true because A is 4 bytes");
_Static_assert(!ttl::models<IsFourBytes, B>::value, "Returns false because B is not 4 bytes");

```
---

[Top](#ttlmodels_if)