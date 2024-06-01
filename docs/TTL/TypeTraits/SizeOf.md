[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Type Traits](./TypeTraitsHeader.md)

# ttl::size_of

`ttl::size_of` is a UnaryTypeTrait.

Provides the member constant `value` equal to the size of the type `T`, as if obtained by a `sizeof` expression.

If the program adds specializations for `ttl::size_of` or `ttl::size_of_v`, the behavior is undefined.

NOTE: It is recommended to prefer this type trait over the `sizeof` operator due to this [bug](https://github.com/microsoft/DirectXShaderCompiler/issues/5553). `ttl::size_of` will succeessfully return the size of every type, whereas, `sizeof` will fail when `T` is an enumeration or contains an enumeration.

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Helper variable template](#helper-variable-template)
4. [Template Parameters](#template-parameters)
5. [Nested Types](#nested-types)
6. [Member Constants](#member-constants)
7. [Example](#example)

## Header

`/Test/TTL/type_traits.hlsli`

## Declaration

```c++
template<typename T>
struct size_of
```

## Helper variable template

```c++
template<typename T>
static const uint size_of_v = size_of<T>::value;
```

## Template Parameters

1. `T` -> a type to check.

## Nested Types

Inherited from [`ttl::integral_constant`](./IntegralConstant.md)
| Name | Definition |
|---------------|----------------------------|
| `value_type`  | `uint`                        |
| `type`        | `integral_constant<uint, value>;` |

## Member Constants
Inherited from [`ttl::integral_constant`](./IntegralConstant.md)

| Name                    | Value |
|-------------------------|-------|
| `static const uint value`  | The size, in bytes, of the type `T` |


## Example

```c++

#include "/Test/TTL/type_traits.hlsli"

enum A
{
    One
};

enum class B
{
    One
};

struct C
{
    int D;
};
                                                                                       
struct D
{
    B data;
};
                                                                                         
_Static_assert(4u == ttl::size_of<A>::value, "Size of unscoped enum is 4");
//_Static_assert(4u == sizeof(A), "This will not compile because A is an enum");

_Static_assert(4u == ttl::size_of<B>::value, "Size of scoped enum is 4");
//_Static_assert(4u == sizeof(B), "This will not compile because B is an enum");

_Static_assert(4u == ttl::size_of<C>::value, "Size of a struct with a single int member is 4");
_Static_assert(4u == ttl::size_of<uint>::value, "Size of uint is 4");
_Static_assert(8u == ttl::size_of<uint64_t>::value, "Size of uint64 is 8");
_Static_assert(2u == ttl::size_of<uint16_t>::value, "Size of uint16_t is 2");
_Static_assert(24u == ttl::size_of<uint64_t3>::value, "Size of a vector of 3 uint64s is 24");
_Static_assert(8u == ttl::size_of<uint16_t4>::value, " Size of a vector of 4 uint16s is 8");

_Static_assert(4u == ttl::size_of<D>::value, "Size of a struct with a single enum member is 4");
//_Static_assert(4u == sizeof(D), "This will not compile because D contains an enum member");

```
---

[Top](#ttlsize_of)