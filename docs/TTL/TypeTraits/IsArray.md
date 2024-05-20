# ttl::is_array

`ttl::is_array` is a UnaryTypeTrait.

Checks whether `T` is an array type. Provides the member constant `value` which is equal to `true`, if `T` is an array type. Otherwise, `value` is equal to `false`.

If the program adds specializations for `ttl::is_array` the behavior is undefined. 

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
struct is_array
```

## Template Parameters

1. `T` -> a type to check.

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
| `static const bool value`  | `true` if `T` is an array type, `false` otherwise   |


## Example

```c++

#include "/Test/TTL/type_traits.hlsli"

_Static_assert(ttl::is_array<A, A>::value, "Returns true because both arguments are A");
_Static_assert(ttl::is_array<A, Alias>::value, "Returns true because we are comparing an alias to A and A");
_Static_assert(!ttl::is_array<A, B>::value, "Returns false because A and B are different types");

using StringLiteralType = __decltype("Hello");
using NotArray = int;
using Array = int[42];

_Static_assert(ttl::is_array<StringLiteralType>::value, "Returns true because a string literal is an array type");
_Static_assert(ttl::is_array<Array>::value, "Returns true because built in arrays are array types");
_Static_assert(!ttl::is_array<NotArray>::value, "Returns false because int is not an array type");

```
---

[Top](#ttlis_array)