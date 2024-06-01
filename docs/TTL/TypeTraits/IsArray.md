[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Type Traits](./TypeTraitsHeader.md)

# ttl::is_array

`ttl::is_array` is a UnaryTypeTrait.

Checks whether `T` is an array type. Provides the member constant `value` which is equal to `true`, if `T` is an array type. Otherwise, `value` is equal to `false`.

If the program adds specializations for `ttl::is_array` or `ttl::is_array_v` the behavior is undefined. 

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Helper variable template](#helper-variable-template)
4. [Template Parameters](#template-parameters)
5. [Nested Types](#nested-types)
6. [Member Constants](#member-constants)
7. [Example](#example)

## Header

`/Test/TTL/type_traits.hlsli`

## Declaration

```c++
template<typename T>
struct is_array
```

## Helper variable template

```c++
template<typename T>
static const bool is_array_v = is_array<T>::value;
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

using StringLiteralType = __decltype("Hello");
using NotArray = int;
using Array = int[42];

_Static_assert(ttl::is_array<StringLiteralType>::value, "Returns true because a string literal is an array type");
_Static_assert(ttl::is_array<Array>::value, "Returns true because built in arrays are array types");
_Static_assert(!ttl::is_array_v<NotArray>, "Returns false because int is not an array type");

```
---

[Top](#ttlis_array)