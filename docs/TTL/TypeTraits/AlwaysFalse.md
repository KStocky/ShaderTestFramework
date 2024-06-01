[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Type Traits](./TypeTraitsHeader.md)

# ttl::always_false

Provides the member constant `value` equal to `false` regardless of the type `T`. This utility template can be used to trigger a `Static_assert` if a particular template is instantiated. It is required to workaround the issue that `_Static_assert(false)` in a template will always fail a build even though the template is not instantiated. This is issue is discussed in [Allowing `static_assert(false)`](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2593r0.html).

If the program adds specializations for `ttl::always_false` or `ttl::always_false_v`, the behavior is undefined. 

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
struct always_false
```

## Helper variable template

```c++
template<typename T>
static const bool always_false_v = always_false<T>::value;
```

## Template Parameters

1. `T` -> A type.

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
| `static const bool value`  | `false`   |


## Example

```c++

#include "/Test/TTL/type_traits.hlsli"

template<typename T>
struct NeverInstantiate
{
    _Static_assert(ttl::always_false_v<T>, "This static assert will only fail a compilation if this struct is ever instantiated");
};

template<typename T>
struct WillAlwaysFailCompilation
{
    _Static_assert(false, "This static assert will always fail a compilation even if this template is never instantiated");
}

_Static_assert(!ttl::always_false<uint>::value, "Always returns false");


```
---

[Top](#ttlalways_false)