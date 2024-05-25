# ttl::is_or_has_enum

`ttl::is_or_has_enum` is a UnaryTypeTrait.

Checks whether `T` is or contains an enumeration type. Provides the member constant `value` which is equal to `true`, if `T` is or contains an enumeration type. Otherwise, value is equal to false.

If the program adds specializations for `ttl::is_or_has_enum`, the behavior is undefined.

It should be noted that this works due to this bug in DXC, [link](https://github.com/microsoft/DirectXShaderCompiler/issues/5553). When (if) this bug is fixed this type trait will no longer be able to implemented in the current way.

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
struct is_or_has_enum
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
| `static const bool value`  | `true` if `T` is or has an enumeration type, `false` otherwise   |


## Example

```c++

#include "/Test/TTL/type_traits.hlsli"

enum A
{
    One
};

enum class B
{
    One
};

struct C
{
    int D;
};
                                                                                       
struct D
{
    B data;
};
                                                                                         
_Static_assert(ttl::is_or_has_enum<A>::value, "true because A is an enumeration");
_Static_assert(ttl::is_or_has_enum<B>::value, "true because B is an enum class");
_Static_assert(!ttl::is_or_has_enum<C>::value, "false because C is a struct that does not contain an enumeration");
_Static_assert(!ttl::is_or_has_enum<uint>::value, "false because uint is not an enumeration");
_Static_assert(ttl::is_or_has_enum<D>::value, "true because D contains an enumeration");

```
---

[Top](#ttlis_or_has_enum)