# Pseudo Concepts

Although HLSL does not have a feature like [C++20 Concepts](https://en.cppreference.com/w/cpp/language/constraints), the TTL provides a mechanism for emulating it. This is possible due to the DXC intrinsic `__decltype` which functions in the same way that [`decltype`](https://en.cppreference.com/w/cpp/language/decltype) in C++ functions. That is, it will return the type of the provided expression. Pseudo Concepts help to provide some of the utility of C++ Concepts for defining constaints on template arguments.

**Contents**
1. [Writing a Pseudo Concept](#writing-a-pseudo-concept)
2. [Concepts with more than one constraint](#concepts-with-more-than-one-constraint)
3. [Evaluating a Pseudo Concept](#evaluating-a-pseudo-concept)
4. [Using composition with Pseudo Concepts](#using-composition-with-pseudo-concepts)
5. [Using Type-Traits with Pseudo Concepts](#using-type-traits-with-pseudo-concepts)
6. [Forming disjunctions](#forming-disjunctions)
7. [Constraining on the type of expressions](#constraining-on-the-type-of-expressions)

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

A Pseudo concept is a non-templated struct which has an undefined `requires` member function. This function is often a function template but is not required to be. The return type of this function is the important bit. It is defined using `__decltype`. The expression that we give to `__decltype` defines the constraints of the pseudo concept. In this case we use [`ttl::declval`](../TypeTraits/DeclVal.md) to get an object of type `T` in this unevaluated context, and then call `Foo` on it. The idea is that if this expression is valid (i.e. an object of type `T` is able to call a member function called `Foo`) then `__decltype` will return a valid type and `requires` will be valid for that type `T`. If, however, `T` does not have a member function called `Foo`, then that expression is not valid which means that the evaluation of `__decltype` is ill-formed, which means that `requires` is not valid for that type.

## Concepts with more than one constraint

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

```c++

struct FourByteType
{
    template<typename T>
    __decltype(
        ttl::models_if<ttl::size_of<T>::value == 4>()
    ) requires();
};

struct FundamentalType
{
    template<typename T>
    __decltype(
        ttl::models_if<
            ttl::fundamental_type_traits<T>::is_fundamental
        >()
    ) requires();
};

```

## Forming disjunctions

Concepts are only satisfied if every comma-separated expression within the `__decltype` expression is satisfied. However, it can sometimes be useful to be able to provide a Concept which is satisfied if ANY of the specified constaints is satisfied. [`ttl::models_if`](./ModelsIf.md) a way of expression disjunctions in this way.

```c++

struct TypeWithFoo
{
    template<typename T>
    __decltype(
        ttl::declval<T>().Foo()
    ) requires();
};

struct TypeWithBar
{
    template<typename T>
    __decltype(
        ttl::declval<T>().Bar()
    ) requires();
};

struct TypeWithFooOrBar
{
    template<typename T>
    __decltype(
        ttl::models_if<
            ttl::models<TypeWithFoo, T>::value ||
            ttl::models<TypeWithBar, T>::value
        >()
    ) requires();
};

struct A
{
    void Foo();
};

struct B
{
    void Bar();
};

_Static_assert(ttl::models<TypeWithFooOrBar, A>::value, "Returns true because A has a Foo");
_Static_assert(ttl::models<TypeWithFooOrBar, B>::value, "Returns true because B has a Bar");

```

## Constraining on the type of expressions

It is often required to not only want a particular member function to exist, but also to want it to return a particular type. [`ttl::models_if_same`](./ModelsIfSame.md) provides a mechanism for doing this:

```c++
struct FooReturnsFloat
{
    template<typename T>
    __decltype(
        ttl::models_if_same<
            float, __decltype(ttl::declval<T>().Foo())
        >()
    ) requires();
};

```

TTL also provides a generalization of [`ttl::models_if_same`](./ModelsIfSame.md) called [`ttl::models_if_pred`](./ModelsIfPred.md) which allows for more general constraints to be placed on the return type of an expression. The following is an example of a concept which is satisfied if a type has a member function called `Foo` which returns either an `int` or a `float`:

```c++
struct FooReturnsFloatOrInt
{
    template<typename T>
    struct FloatOrInt : ttl::integral_constant<bool, ttl::is_same<T, int>::value || ttl::is_same<T, float>::value>{};

    template<typename T>
    __decltype(
        ttl::models_if_pred<
            FloatOrInt, __decltype(ttl::declval<T>().Foo())
        >()
    ) requires();
};

```


[Top](#pseudo-concepts)