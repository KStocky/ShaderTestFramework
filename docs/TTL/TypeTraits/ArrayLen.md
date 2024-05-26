[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Type Traits](./TypeTraitsHeader.md)

# ttl::array_len

"Returns" a value of type [`ttl::integral_constant<uint, N>`](./IntegralConstant.md) that represents the length of the array passed.
Note that `ttl::array_len` can only be used in unevaluated contexts and is not required to be defined; it is an error to evaluate an expression that contains this function.

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
template<typename T, uint N>
integral_constant<uint, N> array_len(T In[N]);
```

## Parameters

1. `In` - An array of type `T` and length `N`

## Return value

Cannot be called and thus never returns a value. The return type is [`ttl::integral_constant<uint, N>`](./IntegralConstant.md) where N is the length of the array. This can be evaluated using the `value` static member constant of the return type. If `T` is not an array type (this can be checked using [`ttl::is_array`](./IsArray.md)) then `N` is `0`.

## Example

```c++

#include "/Test/TTL/type_traits.hlsli"

using StringLiteralType = __decltype(ttl::array_len("Hello"));
using NotArray = __decltype(ttl::array_len(1));
using Array = __decltype(ttl::array_len((int[42])0));

template<typename T, uint ExpectedLength>
struct Tester : ttl::integral_constant<bool, T::value == ExpectedLength>{};

_Static_assert(Tester<StringLiteralType, 6u>::value, "Returns 6 because Hello is 6 characters in length including the null character");
_Static_assert(Tester<Array, 42u>::value, "Returns 42 because Array is an array of 42 ints");
_Static_assert(Tester<NotArray, 0u>::value, "Returns 0 because an int is not an array");

```
---

[Top](#ttlarray_len)