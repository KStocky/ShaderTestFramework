[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Type Traits](./TypeTraitsHeader.md)

# ttl::is_function

`ttl::is_function` is a UnaryTypeTrait.

Checks whether `T` is a function type. Structs with overloaded operator() don't count as function types. Provides the member constant `value` which is equal to `true`, if `T` is a function type. Otherwise, `value` is equal to `false`.

If the program adds specializations for `ttl::is_function`, the behavior is undefined. 

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
template<typename T>
struct is_function;
```

## Template Parameters

1. `T` -> a type to check.

## Specializations

| Description    | Definition                       |
|----------------|----------------------------------|
| `value` is true if `T` is a function that takes zero args    | `template<typename RetType>`<br>`struct is_function<RetType()>` |
| `value` is true if `T` is a function that takes one arg    | `template<typename RetType, typename Arg0>`<br>`struct is_function<RetType(Arg0)>` |
| `value` is true if `T` is a function that takes two args    | `template<typename RetType, typename Arg0, typename Arg1>`<br>`struct is_function<RetType(Arg0, Arg1)>` |
| `value` is true if `T` is a function that takes three args    | `template<typename RetType, typename Arg0, typename Arg1, typename Arg2>`<br>`struct is_function<RetType(Arg0, Arg1, Arg2)>` |
| `value` is true if `T` is a function that takes four args    | `template<typename RetType, typename Arg0, typename Arg1, typename Arg2, typename Arg3>`<br>`struct is_function<RetType(Arg0, Arg1, Arg2, Arg3)>` |
| `value` is true if `T` is a function that takes five args    | `template<typename RetType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>`<br>`struct is_function<RetType(Arg0, Arg1, Arg2, Arg3, Arg4)>` |
| `value` is true if `T` is a function that takes six args    | `template<typename RetType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>`<br>`struct is_function<RetType(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5)>` |

More specializations can be added if we need them...

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

struct Functor
{
    void operator()(){}
};

void Foo(){}
float Bar(int){}

_Static_assert(ttl::is_function<__decltype(Foo)>::value, "Expected the type of Foo to be a function type");
_Static_assert(ttl::is_function<__decltype(Bar)>::value, "Expected the type of Bar to be a function type");

_Static_assert(!ttl::is_function<Functor>::value, "Expected a functors to not be a function");
_Static_assert(!ttl::is_function<int>::value, "Expected int to not be a function");

```
---

[Top](#ttlis_function)