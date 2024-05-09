# ttl::is_invocable_function

Checks whether `T` is a function type that can be called with the supplied argument types. Structs with overloaded operator() don't count as function types. Provides the member constant `value` which is equal to `true`, if `T` is a function type that can be invoked with the supplied argument types. Otherwise, `value` is equal to `false`.

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
template<typename T, typename Arg0 = void, typename Arg1 = void, typename Arg2 = void, typename Arg3 = void, typename Arg4 = void, typename Arg5 = void> 
struct is_invocable_function;
```

## Template Parameters

1. `T` -> a type to check.
2. `Arg0-5` -> Types of the arguments to check

## Specializations

| Description    | Definition                       |
|----------------|----------------------------------|
| `value` is true if `T` is a function that is invocable with zero args    | `template<typename RetType>`<br>`struct is_invocable_function<RetType()>` |
| `value` is true if `T` is a function that is invocable with `Arg0`    | `template<typename RetType, typename Arg0>`<br>`struct is_invocable_function<RetType(Arg0), Arg0>` |
| `value` is true if `T` is a function that is invocable with two args    | `template<typename RetType, typename Arg0, typename Arg1>`<br>`struct is_invocable_function<RetType(Arg0, Arg1), Arg0, Arg1>` |
| `value` is true if `T` is a function that is invocable with three args    | `template<typename RetType, typename Arg0, typename Arg1, typename Arg2>`<br>`struct is_invocable_function<RetType(Arg0, Arg1, Arg2), Arg0, Arg1, Arg2>` |
| `value` is true if `T` is a function that is invocable with four args    | `template<typename RetType, typename Arg0, typename Arg1, typename Arg2, typename Arg3>`<br>`struct is_invocable_function<RetType(Arg0, Arg1, Arg2, Arg3), Arg0, Arg1, Arg2, Arg3>` |
| `value` is true if `T` is a function that is invocable with five args    | `template<typename RetType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>`<br>`struct is_invocable_function<RetType(Arg0, Arg1, Arg2, Arg3, Arg4), Arg0, Arg1, Arg2, Arg3, Arg4>` |
| `value` is true if `T` is a function that is invocable with six args    | `template<typename RetType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>`<br>`struct is_invocable_function<RetType(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5), Arg0, Arg1, Arg2, Arg3, Arg4, Arg5>` |

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
| `static const bool value`  | `true` if `T` is a function type that takes the supplied arguments, `false` otherwise   |


## Example

```c++

#include "/Test/TTL/type_traits.hlsli"

struct Functor
{
    void operator()(){}
};

void Foo(){}
float Bar(int){}

_Static_assert(ttl::is_invocable_function<__decltype(Foo)>::value, "Returns true because Foo is a function that takes 0 args");
_Static_assert(!ttl::is_invocable_function<__decltype(Foo), int>::value, "Returns false because Foo does not take a single parameter of type int");

_Static_assert(!ttl::is_invocable_function<__decltype(Bar)>::value, "Returns false because Bar is not invocable with zero arguments");
_Static_assert(ttl::is_invocable_function<__decltype(Bar), int>::value, "Returns true because Bar is invocable with a single argument of type int");
_Static_assert(!ttl::is_invocable_function<__decltype(Bar), float>::value, "Returns false because Bar does not take a single argument of type float");
_Static_assert(!ttl::is_invocable_function<__decltype(Bar), int, int>::value, "Returns false because Bar does not take two ints");

```
---

[Top](#ttlis_invocable_function)