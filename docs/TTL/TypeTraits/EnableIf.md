# ttl::enable_if

If `InCond` is `true`, `ttl::enable_if` has a public member typedef `type`, equal to `T`; otherwise, there is no member typedef.

This metafunction is a convenient way to leverage SFINAE, in particular for conditionally removing functions from the candidate set based on type traits, allowing separate function overloads or specializations based on those different type traits.

`ttl::enable_if` can be used in many forms, including:

- as an additional function argument (not applicable to most operator overloads),
- as a return type (not applicable to constructors and destructors),
- as a class template or function template parameter. 

If the program adds specializations for `ttl::enable_if`, the behavior is undefined. 

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Template Parameters](#template-parameters)
4. [Member Types](#member-types)
5. [Example](#example)

## Header

`/Test/TTL/type_traits.hlsli`

## Declaration

```c++
template<bool InCond, typename T = void>
struct enable_if
```

## Template Parameters

1. `InCond` -> boolean condition.
2. `T` -> Type of `type` if `InCond` is true.

## Member Types

| Name | Definition |
|---------------|----------------------------|
| `type`        | either `T` if `InCond` is `true`, or it does not exist if `InCond` is `false` |

## Example

```c++

#include "/Test/TTL/type_traits.hlsli"

void Foo()
{
}

template<typename T, typename = void>
struct A
{
    static const uint value = 0;
};

template<typename T>
struct A<T, typename ttl::enable_if<ttl::is_array<T>::value>::type>
{
    static const uint value = 1;
};

template<typename T>
struct A<T, typename ttl::enable_if<ttl::is_function<T>::value>::type>
{
    static const uint value = 2;
};

_Static_assert(A<int>::value == 0, "value is 0 because int is neither an array or a function so it takes the primary template");
_Static_assert(A<float[42]>::value == 1, "value is 1 because float[42] is an array so it takes the first specialization");
_Static_assert(A<__decltype(Foo)>::value == 2, "value is 2 because Foo is a function so it takes the second specialization");

```
---

[Top](#ttlenable_if)