# ttl::declval

"Returns" a value of type `T`, making it possible to use member functions and pass objects of this type in the operand of the `__decltype` specifier

Note that `ttl::declval` can only be used in unevaluated contexts and is not required to be defined; it is an error to evaluate an expression that contains this function.

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Parameters](#parameters)
4. [Return value](#return-value)
5. [Example](#example)

## Header

`/Test/TTL/type_traits.hlsli`

## Declaration

```c++
template<typename T>
T declval();
```

## Parameters

None

## Return value

Cannot be called and thus never returns a value. The return type is `T`. As `T` is a value type, it is not possible for `ttl::declval` to be used with array types.
e.g. `ttl::declval<int[5]>()` is an ill-formed expression.

## Example

```c++

#include "/Test/TTL/type_traits.hlsli"

struct A
{
};

struct B
{
    int Foo();
    float Bar(A);
};

// var1 is an int.
__decltype(ttl::declval<B>().Foo()) var1 = 42;

// var2 is a float.
__decltype(ttl::declval<B>().Bar(ttl::declval<A>())) var2 = 2.0;

```
---

[Top](#ttldeclval)