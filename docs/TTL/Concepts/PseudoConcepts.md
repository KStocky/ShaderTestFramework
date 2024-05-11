# Pseudo Concepts

Although HLSL does not have a feature like [C++20 Concepts](https://en.cppreference.com/w/cpp/language/constraints), the TTL provides a mechanism for emulating it. This is possible due to the DXC intrinsic `__decltype` which functions in the same way that [`decltype`](https://en.cppreference.com/w/cpp/language/decltype) in C++ functions. That is, it will return the type of the provided expression. Pseudo Concepts help to provide some of the utility of C++ Concepts for defining constaints on template arguments.

## Writing a Pseudo Concept

The following is a concept for defining the requirement that a type has to have the `Foo` member function.

```c++

struct TypeWithFoo
{
    template<typename T>
    __decltype(
        ttl::declval<T>().Foo()
    ) requires();
};

```

A Pseudo concept is a non-templated struct which has an undefined `requires` member function. This function is often a function template but is not required to be. The return type of this function is the important bit. It is defined using `__decltype`. The expression that we give to `__decltype` the constraints of the pseudo concept. In this case we use [`ttl::declval`](../TypeTraits/DeclVal.md) to get an object of type `T` in this unevaluated context, and then call `Foo` on it. The idea is that if this expression is valid (i.e. the type `T` is able to call a member function called `Foo`) then `__decltype` will return a valid type and `requires` will be valid for that type `T`. If, however, `T` does not have a member function called `Foo`, then that expression is not valid which means that the evaluation of `__decltype` is ill-formed, which means that `requires` is not valid for that type.

More than one constraint can be defined in a pseudo concept by separating each constraint with a comma

```c++

// This is a concept which succeeds if T has the following member functions:
// 1. Foo()
// 2. Bar(int)
struct TypeWithFooAndBar
{
    template<typename T>
    __decltype(
        ttl::declval<T>().Foo(),
        ttl::declval<T>().Bar(ttl::declval<int>())
    ) requires();
};

```

## Evaluating a Pseudo Concept

TTL provides a meta-function called [`ttl::models`](./Models.md) which will take a pseudo concept as it's first template parameter and then take up to 6 template arguments to evaluate the pseudo concept. Below is an example of determining whether two structs model the concept `TypeWithFoo` (defined above).

```c++

#include "/Test/TTL/models.hlsli"

struct A
{
    void Foo();
};

struct B{};

_Static_assert(ttl::models<TypeWithFoo, A>::value, "Returns true because A has a member function called Foo");
_Static_assert(ttl::models<TypeWithFoo, B>::value, "Returns false because B does not have a member function called Foo");

```

## Using composition with Pseudo Concepts

TTL provides [`ttl::models_refines`](./ModelsRefines.md) as a way of supporting composing pseudo concepts with other pseudo concepts.
Below is another way that the pseudo concept `TypeWithFooAndBar` could be defined

```c++

// This is a concept which succeeds if T models the concept TypeWithFoo and has a member function Bar(int)
struct TypeWithFooAndBar
{
    template<typename T>
    __decltype(
        ttl::models_refines<TypeWithFoo, T>(),
        ttl::declval<T>().Bar(ttl::declval<int>())
    ) requires();
};

```

## Using Type-Traits with Pseudo Concepts

It can often be useful to define your pseudo concept in terms of a type trait. TTL provides [`ttl::models_if`](./ModelsIf.md) to do this.



[Top](#pseudo-concepts)