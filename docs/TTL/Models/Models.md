[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Models](./ModelsHeader.md)

# ttl::models

Utility metafunction that will evalulate a [pseudo concept](./PseudoConcepts.md). It checks whether a series of up to 6 template arguments satisfy the constraints defined by the [pseudo concept](./PseudoConcepts.md) `Concept`. Provides the member constant `value` which is equal to `true`, if the template arguments satisfy the pseudo concept. Otherwise, `value` is equal to `false`.

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Template Parameters](#template-parameters)
4. [Member Constants](#member-constants)
5. [Example](#example)

## Header

`/Test/TTL/models.hlsli`

## Declaration

```c++
template <
        typename Concept, 
        typename Arg0, typename Arg1 = ttl_detail::null_type, typename Arg2 = ttl_detail::null_type,
        typename Arg3 = ttl_detail::null_type, typename Arg4 = ttl_detail::null_type, typename Arg5 = ttl_detail::null_type
    >
    struct models
```

## Template Parameters

1. `Concept` -> The pseudo concept to evaluate.
2. `Arg0-5` -> Types to supply to the pseudo concept for evaluation

## Member Constants

| Name                    | Value |
|-------------------------|-------|
| `static const bool value`  | `true` if the pseudo concept `Concept` is satisfied, `false` otherwise   |


## Example

```c++

#include "/Test/TTL/models.hlsli"

// Pseudo concept for checking if objects of two types can be added together
struct Addable
{
    template<typename T, typename U>
    __decltype(
        ttl::declval<T>() + ttl::declval<U>()
    ) requires();
};

struct A{};

_Static_assert(ttl::models<Addable, int, int>::value, "Returns true because you can add two ints together");
_Static_assert(ttl::models<Addable, float, float>::value, "Returns true because you can add two floats together");
_Static_assert(!ttl::models<Addable, A, float>::value, "Returns false because you can not add an A and a float");

```
---

[Top](#ttlmodels)