# ttl::string_literal

This is a [pseudo-concept](./PseudoConcepts.md) for use with [`ttl::models`](./Models.md).

**Contents**
1. [Header](#header)
2. [Requirements](#requirements)
3. [Definition](#definition)
4. [Example](#example)

## Header

`/Test/TTL/concepts.hlsli`

## Requirements

The concept is modeled if the type, `T` is a string literal.

## Definition

```c++
struct string_literal
{
    template<typename T>
    __decltype(
        models_if<(__decltype(ttl_detail::array_len(declval<T>()))::value > 0)>()
    ) requires();
};
```

## Example

```c++

#include "/Test/TTL/concepts.hlsli"
#include "/Test/TTL/models.hlsli"

struct A
{
};

_Static_assert(!ttl::models<ttl::string_literal, A[6]>::, "not a string literal");
_Static_assert(!ttl::models<ttl::string_literal, A>::value, "not a string literal");

_Static_assert(ttl::models<ttl::string_literal, __decltype("Hello")>::value, "Is a string literal");

```
---

[Top](#ttlstring_literal)