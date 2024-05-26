[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Concepts](./ConceptsHeader.md)

# ttl::equality_comparable, ttl::equality_comparable_with

These are [pseudo-concepts](../PseudoConcepts.md) for use with [`ttl::models`](../Models/Models.md).

**Contents**
1. [Header](#header)
2. [Requirements](#requirements)
3. [Definition](#definition)
4. [Example](#example)

## Header

`/Test/TTL/concepts.hlsli`

## Requirements

1. `ttl::equality_comparable` - Modelled if the type `T` defines `bool operator==(T)` and `bool operator!=(T)`
2. `ttl::equality_comparable_with` - Modelled if the type `T` defines `bool operator==(U)` and `bool operator!=(U)`

## Definition

### 1. ttl::equality_comparable

```c++
struct equality_comparable
{
    template<typename T>
    __decltype(
        models_refines<equality_comparable_with, T, T>()
    ) requires();
};
```

### 2. ttl::equality_comparable_with

```c++
struct equality_comparable_with
{
    template<typename T, typename U>
    __decltype(
        models_if_same<bool, __decltype(declval<T>() == declval<U>())>(),
        models_if_same<bool, __decltype(declval<T>() != declval<U>())>()
    ) requires();
};
```


## Example

```c++

#include "/Test/TTL/concepts.hlsli"
#include "/Test/TTL/models.hlsli"

struct A
{
    bool operator==(A);
    bool operator!=(A);
};

struct B
{
    bool operator==(A);
    bool operator!=(A);
};

_Static_assert(ttl::models<ttl::equality_comparable, A>::value, "Returns true because A is equality comparable");
_Static_assert(!ttl::models<ttl::equality_comparable, B>::value, "Returns false because B is not equality comparable");

_Static_assert(ttl::models<ttl::equality_comparable_with, B, A>::value, "Returns true because a B can be equality compared with an A");
_Static_assert(!ttl::models<ttl::equality_comparable_with, A, B>::value, "Returns false because an A can not be equality compared with a B");


```
---

[Top](#ttlequality_comparable-ttlequality_comparable_with)