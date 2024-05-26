[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Type Traits](./TypeTraitsHeader.md)

# ttl::is_base_of

`ttl::is_base_of` is a BinaryTypeTrait.

If `Derived` is derived from `Base` or if both are the same struct, provides the member constant `value` equal to `true`. Otherwise `value` is `false`.

If the program adds specializations for `ttl::is_base_of`, the behavior is undefined.

NOTE: Although no struct is its own base, `ttl::is_base_of<T, T>::value` is `true` because the intent of the trait is to model the "is-a" relationship, and `T` is a `T`. Despite that, `ttl::is_base_of<int, int>::value` is `false` because only structs participate in the relationship that this trait models.

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
struct is_base_of
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
| `static const bool value`  | `true` if `Derived` is derived from `Base` or if both are the same class (in both cases ignoring cv-qualification), `false` otherwise   |


## Example

```c++

#include "/Test/TTL/type_traits.hlsli"

    struct A{};
    struct B : A{};
    struct C : B{};

    struct D{};

    _Static_assert(ttl::is_base_of<A, B>::value, "true because A is the base class of B");
    _Static_assert(ttl::is_base_of<A, C>::value, "true because C derives from B which derives from A");
    _Static_assert(ttl::is_base_of<A, A>::value, "true because A is an A");

    _Static_assert(!ttl::is_base_of<int, int>::value, "false because only struct types can have an is-a relationship");
    _Static_assert(!ttl::is_base_of<A, D>::value, "false because D does not have a base class of type A");
    _Static_assert(!ttl::is_base_of<int, A>::value, "false because int is not a struct type and because there is no is-a relationship");

```
---

[Top](#ttlis_base_of)