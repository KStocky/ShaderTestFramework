[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Concepts](./ConceptsHeader.md)

# ttl::invocable_functor, ttl::invocable

These are [pseudo-concepts](./PseudoConcepts.md) for use with [`ttl::models`](./Models.md).

**Contents**
1. [Header](#header)
2. [Requirements](#requirements)
3. [Definition](#definition)
4. [Example](#example)

## Header

`/Test/TTL/concepts.hlsli`

## Requirements

1. `ttl::invocable_functor` - Modelled if the type `F` is a struct that defines `operator()` which is invocable using the specified argument types. A range of [0,5] argument types can be supplied.
2. `ttl::invocable` - Modelled if the type `F` models either a `ttl::invocable_functor`, or satisfies the type trait, [`ttl::is_invocable_function`](../TypeTraits/IsInvocableFunction.md), with the specified argument types. A range of [0,5] argument types can be supplied.

## Definition

### 1. ttl::invocable_functor

```c++
struct invocable_functor
{
    template<
        typename F
        >
    __decltype(
        declval<F>()()
    ) requires();

    template<
        typename F,
        typename Arg0
        >
    __decltype(
        declval<F>()(declval<Arg0>())
    ) requires();

    // More declarations to support up to and including 5 arguments
};
```

### 2. ttl::invocable

```c++
struct invocable
{
    template<
        typename F
        >
    __decltype(
        models_if<
            models<invocable_functor, F>::value ||
            is_invocable_function<F>::value
        >()
    ) requires();

    template<
        typename F,
        typename Arg0
        >
    __decltype(
        models_if<
            models<invocable_functor, F, Arg0>::value ||
            is_invocable_function<F, Arg0>::value
        >()
    ) requires();

    // More declarations to support up to and including 5 arguments
};
```


## Example

```c++

#include "/Test/TTL/concepts.hlsli"
#include "/Test/TTL/models.hlsli"

float Foo(int);

struct Bar
{
    void operator()(int);
};

_Static_assert(!ttl::models<ttl::invocable_functor, __decltype(Foo), int>::value, "Returns false because Foo is a function, not a functor");
_Static_assert(!ttl::models<ttl::invocable_functor, Bar>::value, "Returns false because Bar does not define operator() which takes zero arguments");
_Static_assert(ttl::models<ttl::invocable_functor, Bar, int>::value, "Returns true because Bar does define operator() which takes a single argument of type int");

_Static_assert(!ttl::models<ttl::invocable, __decltype(Foo)>::value, "Returns false because Foo is not invocable with zero arguments");
_Static_assert(ttl::models<ttl::invocable, __decltype(Foo), int>::value, "Returns true because Foo is invocable with a single argument of type int");
_Static_assert(!ttl::models<ttl::invocable, __decltype(Bar)>::value, "Returns false because Bar is not invocable with zero arguments");
_Static_assert(ttl::models<ttl::invocable, __decltype(Bar), int>::value, "Returns true because Bar is invocable with a single argument of type int");


```
---

[Top](#ttlinvocable_functor-ttlinvocable)