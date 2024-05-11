# ttl::is_same

If `T` and `U` name the same type, provides the member constant `value` equal to `true`. Otherwise `value` is `false`. 

If the program adds specializations for `ttl::is_same`, the behavior is undefined. 

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
template<typename T, typename U>
struct is_same
```

## Template Parameters

1. `T` -> First type to compare.
2. `U` -> Second type to compare.

## Nested Types

Inherited from [`ttl::integral_constant`](./IntegralConstant.md)
| Name | Definition |
|---------------|----------------------------|
| `value_type`  | `bool`                        |
| `type`        | `integral_constant<bool, value>;` |

## Member Constants
Inherited from [`ttl::integral_constant`](./IntegralConstant.md)

| Name                    | Value |
|-------------------------|-------|
| `static const bool value`  | `true` if `T` is a function type, `false` otherwise   |


## Example

```c++

#include "/Test/TTL/type_traits.hlsli"

struct A{};
struct B{};

using Alias = A;

_Static_assert(ttl::is_same<A, A>::value, "Returns true because both arguments are A");
_Static_assert(ttl::is_same<A, Alias>::value, "Returns true because we are comparing an alias to A and A");
_Static_assert(!ttl::is_same<A, B>::value, "Returns false because A and B are different types");

```
---

[Top](#ttlis_same)