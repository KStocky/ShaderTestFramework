[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Models](./ModelsHeader.md)

# ttl::models_refines

A utility function to be used within the definition of a [pseudo concept](../PseudoConcepts.md). `ttl::models_refines` enables a pseudo concept to include another pseudo concept via composition.

Note: `ttl::models_refines` is not defined, so it only allowed to be used in an unevaluated context such as within a `__decltype` expression.

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Template Parameters](#template-parameters)
4. [Parameters](#parameters)
5. [Return Value](#return-value)
6. [Example](#example)

## Header

`/Test/TTL/models.hlsli`

## Declaration

```c++

template<
    typename Concept, 
    typename Arg0, typename Arg1 = ttl_detail::null_type, typename Arg2 = ttl_detail::null_type,
    typename Arg3 = ttl_detail::null_type, typename Arg4 = ttl_detail::null_type, typename Arg5 = ttl_detail::null_type
> typename enable_if<models<Concept, Arg0, Arg1, Arg2, Arg3, Arg4, Arg5>::value>::type models_refines();

```

## Template Parameters

1. `Concept` -> The pseudo concept to evaluate.
2. `Arg0-5` -> Types to supply to the pseudo concept for evaluation

## Parameters

None

## Return value

Cannot be called and thus never returns a value. The return type is of type `void` if the supplied argument types satisfy the psuedo concept. Otherwise, the return type is not defined.


## Example

```c++

#include "/Test/TTL/models.hlsli"

struct HasFoo
{
    template<typename T>
    __decltype(
        ttl::declval<T>().Foo()
    ) requires();
};

struct HasBar
{
    template<typename T>
    __decltype(
        ttl::declval<T>().Bar()
    ) requires();
};

struct HasFooAndBar
{
    template<typename T>
    __decltype(
        ttl::models_refines<HasFoo, T>(),
        ttl::models_refines<HasBar, T>()
    ) requires();
}

struct A
{
    void Foo();
};

struct B
{
    void Bar();
};

struct C
{
    void Foo();
    void Bar();
};

_Static_assert(!ttl::models<HasFooAndBar, A>::value, "Returns false because A only has Foo");
_Static_assert(!ttl::models<HasFooAndBar, B>::value, "Returns false because B only has Bar");
_Static_assert(ttl::models<HasFooAndBar, C>::value, "Returns true because C models both HasFoo and HasBar");

```
---

[Top](#ttlmodels_refines)