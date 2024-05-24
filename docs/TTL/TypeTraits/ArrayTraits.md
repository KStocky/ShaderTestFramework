# ttl::array_traits

Provides array property information on a type `T`. Provides information on the element type and size.

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Template Parameters](#template-parameters)
4. [Member Types](#member-types)
5. [Member Constants](#member-constants)
6. [Example](#example)

## Header

`/Test/TTL/type_traits.hlsli`

## Declaration

```c++
template<typename T>
struct array_traits
```

## Template Parameters

1. `T` -> Type to check.

## Member Types

| Name | Definition |
|---------------|----------------------------|
| `element_type`        | If `T` is an array, this is the element type of the array, `void` otherwise |

## Member Constants

| Name                    | Value |
|-------------------------|-------|
| `static const bool is_array`  | `true` if `T` is an array, `false` otherwise   |
| `static const uint size`  | length of the array if `T` is an array, `0` otherwise   |


## Example

```c++

#include "/Test/TTL/type_traits.hlsli"

using IsArray = ttl::array_traits<float2[4]>;
using NotArray = ttl::array_traits<int>;

_Static_assert(ttl::is_same<IsArray::element_type, float2>::value, "Returns true because the element type is float2");
_Static_assert(ttl::is_same<NotArray::element_type, void>::value, "Returns true because int is not an array and so element_type is defined as void");

_Static_assert(IsArray::is_array, "Returns true because float2[4] is an array");
_Static_assert(!NotArray::is_array, "Returns false because int is not an array");

_Static_assert(IsArray::size == 4u, "the size of float2[4] is 4 because the array has 4 elements");
_Static_assert(NotArray::size == 0u, "the size is 0 int is not an array");

```
---

[Top](#ttlarray_traits)