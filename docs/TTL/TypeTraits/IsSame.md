[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Type Traits](./TypeTraitsHeader.md)

# ttl::is_same

If `T` and `U` name the same type, provides the member constant `value` equal to `true`. Otherwise `value` is `false`. 

If the program adds specializations for `ttl::is_same`, the behavior is undefined. 

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
template<typename T, typename U>
struct is_same
```

## Helper variable template

```c++
template<typename T, typename U>
static const bool is_same_v = is_same<T, U>::value;
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
| `static const bool value`  | `true` if `T` and `U` are the same type, `false` otherwise   |


## Example

```c++

#include "/Test/TTL/type_traits.hlsli"

struct A{};
struct B{};

using Alias = A;

_Static_assert(ttl::is_same<A, A>::value, "Returns true because both arguments are A");
_Static_assert(ttl::is_same<A, Alias>::value, "Returns true because we are comparing an alias to A and A");

// variable template
_Static_assert(!ttl::is_same_v<A, B>, "Returns false because A and B are different types");

```
---

[Top](#ttlis_same)