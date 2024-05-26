[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Type Traits](./TypeTraitsHeader.md)

# ttl::fundamental_type_traits

Provides fundamental type property information on a type `T`. A fundamental type is a scalar, vector or matrix type of any of the following:
- bool
- int16_t, int32_t, int64_t
- uint16_t, uint32_t, uint64_t
- float16_t, float32_t, float64_t

NOTE: This applied to any alias of these types. e.g. The following types are equivilent:
- `float4x4`
- `float32_t4x4`
- `matrix<float, 4, 4>`

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
struct fundamental_type_traits
```

## Template Parameters

1. `T` -> Type to check.

## Member Types

| Name | Definition |
|---------------|----------------------------|
| `base_type`        | If `T` is a fundamental type, this is the corresponding scalar type of `T`, `void` otherwise. |

## Member Constants

| Name                    | Value |
|-------------------------|-------|
| `static const bool is_fundamental`  | `true` if `T` is a fundamental type, `false` otherwise   |
| `static const uint dim0`  | The length of the first dimension if `T` is a fundamental type, `0` otherwise |
| `static const uint dim1`  | The length of the second dimension if `T` is a fundamental type, `0` otherwise |

## Example

```c++

#include "/Test/TTL/type_traits.hlsli"

struct A
{
    int data;
};

using ATraits = ttl::fundamental_type_traits<A>;
using ScalarTraits = ttl::fundamental_type_traits<float>;
using VectorTraits = ttl::fundamental_type_traits<int16_t3>;
using MatrixTraits = ttl::fundamental_type_traits<matrix<bool, 2, 3> >;

_Static_assert(!ATraits::is_fundamental, "Returns false because A is not a fundamental type");
_Static_assert(ScalarTraits::is_fundamental, "Returns true because float is a fundamental type");
_Static_assert(VectorTraits::is_fundamental, "Returns true because int16_t3 is a fundamental type");
_Static_assert(MatrixTraits::is_fundamental, "Returns true because bool2x3 is a fundamental type");

_Static_assert(ATraits::dim0 == 0, "Returns 0 because A is not a fundamental type");
_Static_assert(ScalarTraits::dim0 == 1, "Returns 1 because float has 1 element in the first dimension");
_Static_assert(VectorTraits::dim0 == 3, "Returns 3 because int16_t3 has 3 elements in the first dimension");
_Static_assert(MatrixTraits::dim0 == 2, "Returns 2 because bool2x3 has 2 elements in the first dimension");

_Static_assert(ATraits::dim1 == 0, "Returns 0 because A is not a fundamental type");
_Static_assert(ScalarTraits::dim1 == 1, "Returns 1 because float has 1 element in the second dimension");
_Static_assert(VectorTraits::dim1 == 1, "Returns 1 because int16_t3 has 1 element in the second dimension");
_Static_assert(MatrixTraits::dim1 == 3, "Returns 3 because bool2x3 has 3 elements in the second dimension");

_Static_assert(ttl::is_same<void, ATraits::base_type>::value, "Base type is void because A is not a fundamental type");
_Static_assert(ttl::is_same<float, ScalarTraits::base_type>::value, "Base type is float");
_Static_assert(ttl::is_same<int16_t, VectorTraits::base_type>::value, "Base type is int16_t");
_Static_assert(ttl::is_same<bool, MatrixTraits::base_type>::value, "Base type is bool");

```
---

[Top](#ttlfundamental_type_traits)