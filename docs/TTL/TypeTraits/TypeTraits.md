# Type Traits

The TTL (Test Template Library) provides a number of type traits in the [`type_traits.hlsli`](../../../src/ShaderTestFramework/Shader/TTL/type_traits.hlsli) header. These operate in the same way as [C++ type traits](https://en.cppreference.com/w/cpp/header/type_traits).

## Structs

### Helper Structs

| | |
|--------------|---------------|
| [`integral_constant`](./IntegralConstant.md) | compile-time constant of specified type with specified value |
| [`true_type`](./IntegralConstant.md#specializations) | [`ttl::integral_constant<bool, true>`](./IntegralConstant.md) |
| [`false_type`](./IntegralConstant.md#specializations) | [`ttl::integral_constant<bool, false>`](./IntegralConstant.md) |

### Primary type categories

| | |
|-|-|
| [`is_array`](./IsArray.md) | checks if a type is an array type |
| [`is_function`](./IsFunction.md) | checks if a type is a function type |
| [`is_or_has_enum`](./IsOrHasEnum.md) | checks if a type is an enumeration type or contains an enumeration type |

### Property queries

| | |
|-|-|
| [`align_of`](./AlignOf.md) | obtains the alignment of a type |
| [`array_traits`](./ArrayTraits.md) | Provides information on the size and element types of arrays |
| [`container_traits`](./ContainerTraits.md) | Provides information on the properties of resource buffers and arrays |
| [`fundamental_type_traits`](./FundamentalTypeTraits.md) | Provides information on the properties of fundamental types |
| [`size_of`](./SizeOf.md) | obtains the size of a type |


### Type relationships

| | |
|-|-|
| [`is_same`](./IsSame.md) | checks if two types are the same |
| [`is_base_of`](./IsBaseOf.md) | checks if a type is derived from the other type |
| [`is_invocable_function`](./IsInvocableFunction.md) | checks if a function can be called with the given argument types |


### Miscellaneous transformations

| | |
|-|-|
| [`enable_if`](./EnableIf.md) | conditionally removes a function overload or template specialization from overload resolution  |
| [`void_t`](./VoidT.md) | alias template that maps a sequence of types to void |

## Functions

| | |
|-|-|
| [`array_len`](./ArrayLen.md) | Returns the length of an array |
| [`declval`](./DeclVal.md) | Returns an object of type `T` for use in an unevaluated context |



