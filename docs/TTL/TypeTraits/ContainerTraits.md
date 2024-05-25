# ttl::container_traits

Provides container property information on a type `T`. A container is an array or a buffer resource. Provides information such as element type and whether it is writable, among other information.

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
struct container_traits
```

## Template Parameters

1. `T` -> Type to check.

## Member Types

| Name | Definition |
|---------------|----------------------------|
| `element_type`        | If `T` is a container, this is the element type of the container. If `T` is a byte address buffer, it is `uint`. `void` otherwise. |
| `type` | `T` if `T` is a container, `void` otherwise |

## Member Constants

| Name                    | Value |
|-------------------------|-------|
| `static const bool is_container`  | `true` if `T` is a container, `false` otherwise   |
| `static const uint is_writable`  | `true` if `T`'s elements can be written to, `false` otherwise   |
| `static const uint is_byte_address`  | `true` if `T` is a byte address buffer, `false` otherwise   |
| `static const uint is_structured`  | `true` if `T` is a structured buffer, `false` otherwise   |
| `static const uint is_resource`  | `true` if `T` is a resource, `false` otherwise (i.e. is an array)   |

## Example

```c++

#include "/Test/TTL/type_traits.hlsli"

struct A
{
    int data;
};

using ReadOnlyStructuredBufferTraits = ttl::container_traits<StructuredBuffer<A> >;
using ArrayContainerTraits = ttl::container_traits<float[32]>;
using AContainerTraits = ttl::container_traits<A>;

_Static_assert(ReadOnlyStructuredBufferTraits::is_container, "Returns true because a structured buffer is a container");
_Static_assert(ArrayContainerTraits::is_container, "Returns true because an array is a container");
_Static_assert(!AContainerTraits::is_container, "Returns false because A is not a container");

_Static_assert(!ReadOnlyStructuredBufferTraits::is_writable, "Returns false because a structured buffer is not writable.");
_Static_assert(ArrayContainerTraits::is_writable, "Returns true because a non const array is writable");

_Static_assert(ReadOnlyStructuredBufferTraits::is_resource, "Returns true because a structured buffer is a resource type.");
_Static_assert(!ArrayContainerTraits::is_resource, "Returns false because an array is not a resource");


```
---

[Top](#ttlcontainer_traits)