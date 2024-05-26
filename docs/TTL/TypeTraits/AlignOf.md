# ttl::align_of

`ttl::align_of` is a UnaryTypeTrait.

Provides the member constant `value` equal to the alignment of the type `T`.

If the program adds specializations for `ttl::align_of`, the behavior is undefined.

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Template Parameters](#template-parameters)
4. [Nested Types](#nested-types)
5. [Member Constants](#member-constants)
6. [Example](#example)

## Header

`/Test/TTL/type_traits.hlsli`

## Declaration

```c++
template<typename T>
struct align_of
```

## Template Parameters

1. `T` -> a type to check.

## Nested Types

Inherited from [`ttl::integral_constant`](./IntegralConstant.md)
| Name | Definition |
|---------------|----------------------------|
| `value_type`  | `uint`                        |
| `type`        | `integral_constant<uint, value>;` |

## Member Constants
Inherited from [`ttl::integral_constant`](./IntegralConstant.md)

| Name                    | Value |
|-------------------------|-------|
| `static const uint value`  | The alignment of the type `T` |


## Example

```c++

#include "/Test/TTL/type_traits.hlsli"

struct A
{
    int16_t d1;
    int16_t d2;
    int16_t d3;
    int16_t d4;
};

struct B
{
    int32_t d1;
    int32_t d2;
    int32_t d3;
    int32_t d4;
};

struct C
{
    int64_t d1;
    int64_t d2;
    int64_t d3;
    int64_t d4;
};
                                                                                       
struct D
{
    int64_t d1;
    int32_t d2;
    int16_t d3;
};
                                                                                         
_Static_assert(2u == ttl::align_of<A>::value, "Alignment is 2");
_Static_assert(4u == ttl::align_of<B>::value, "Alignment is 4");
_Static_assert(8u == ttl::align_of<C>::value, "Alignment is 8");
_Static_assert(8u == ttl::align_of<D>::value, "Alignment is 8");

_Static_assert(4u == ttl::align_of<int[8]>::value, "Alignment is 4");


```
---

[Top](#ttlalign_of)