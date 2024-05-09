# ttl::void_t

Utility metafunction that maps a sequence of up to 10 types to the type `void`. This metafunction is a convenient way to leverage SFINAE, in particular for conditionally removing functions from the candidate set based on whether an expression is valid in the unevaluated context (such as operand to `__decltype` expression), allowing to exist separate function overloads or specializations based on supported operations.

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Examples](#examples)<br>
a.[Detecting the presence of nested type members](#1-detecting-the-presence-of-nested-type-members)<br>
b.[Detecting if an expression is valid using `ttl::declval`](#2-detecting-if-an-expression-is-valid-using-ttldeclval)

## Header

`/Test/TTL/type_traits.hlsli`

## Declaration

```c++
template<
        typename T0 = void, typename T1 = void , typename T2 = void, typename T3 = void, typename T4 = void,
        typename T5 = void, typename T6 = void , typename T7 = void, typename T8 = void, typename T9 = void
        >
    using void_t = void;
```

## Examples

### 1. Detecting the presence of nested type members

```c++
#include "/Test/TTL/type_traits.hlsli"

// primary template handles types that have no nested ::type member:
template<typename, typename = void>
struct has_type_member : ttl::false_type {};
 
// specialization recognizes types that do have a nested ::type member:
template<typename T>
struct has_type_member<T, ttl::void_t<typename T::type> > : ttl::true_type {};

struct A
{
    using type = int;
};

struct B
{
    using other_type = float;
};

_Static_assert(has_type_member<A>::value, "A has a nested type named type so this will be true");
_Static_assert(!has_type_member<B>::value, "B does not have a nested type named type so this will be false");

```

### 2. Detecting if an expression is valid using [`ttl::declval`](./DeclVal.md)

```c++

#include "/Test/TTL/type_traits.hlsli"

// primary template handles types that do not support pre-increment:
template<class, class = void>
struct has_call_operator_member : ttl::false_type {};
 
// specialization recognizes types that do support pre-increment:
template<class T>
struct has_call_operator_member<T,
           ttl::void_t<__decltype( ttl::declval<T>()() )>
       > : ttl::true_type {};

struct A
{
    void operator()(){}
};

struct B
{
    void Foo(){}
};

_Static_assert(has_call_operator_member<A>::value, "A has overloaded the call operator so this will be true");
_Static_assert(!has_call_operator_member<B>::value, "B has not overloaded the call operator so this will be false");

```
---

[Top](#ttlvoid_t)