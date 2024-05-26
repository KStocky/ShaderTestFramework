[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Type Traits](./TypeTraitsHeader.md)

# ttl::integral_constant

`ttl::integral_constant` wraps a static constant of specified type. It is the base class for the ttl type traits.

If the shader adds specializations for ttl::integral_constant, the behavior is undefined.

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Template Parameters](#template-parameters)
4. [Specializations](#specializations)
5. [Nested Types](#nested-types)
6. [Member Constants](#member-constants)
7. [Example](#example)

## Header

`/Test/TTL/type_traits.hlsli`

## Declaration

```c++
template<typename T, T v>
struct integral_constant
```

## Template Parameters

1. `T` -> The type of the constant.
2. `v` -> The value of the constant.

## Specializations

| Name           | Definition                       |
|----------------|----------------------------------|
| `true_type`    | `integral_constant<bool, true>;` |
| `false_type`   | `integral_constant<bool, false>;`|

## Nested Types

| Name | Definition |
|---------------|----------------------------|
| `value_type`  | `T`                        |
| `type`        | `integral_constant<T, v>;` |

## Member Constants

| Name                    | Value |
|-------------------------|-------|
| `static const T value`  | `v`   |


## Example

```c++

#include "/Test/TTL/type_traits.hlsli"

using Two = ttl::integral_constant<int, 2>;
using Five = ttl::integral_constant<int, 5>;

_Static_assert(!ttl::is_same<Two, Five>::value, "Expected these types to not be the same");

_Static_assert(Two::value == 2, "Expected the value to be 2");
_Static_assert(Five::value == 5, "Expected the value to be 5");

_Static_assert(Two::value == 2, "Expected the value to be 2");
_Static_assert(Five::value + Two::value == 7, "Expected the answer to be 7");

_Static_assert(Two::value == 2, "Expected the value to be 2");
_Static_assert(Five::value == 5, "Expected the value to be 5");

_Static_assert(ttl::is_same<typename Two::value_type, int>::value, "Expected the value to be of type int");

```
---

[Top](#ttlintegral_constant)