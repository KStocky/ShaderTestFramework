[Reference](../ShaderTestFramework.md) -> [Test Template Library](./TTL.md)

# ttl::container_wrapper

Shader Test Framework provides a wrapper around various types of containers that you might find in HLSL. This is useful for providing a unified interface that will work for any container in HLSL.

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Template Parameters](#template-parameters)
4. [Member Types](#member-types)
5. [Member Constants](#member-constants)
6. [Member Variables](#member-variables)
7. [Member Functions](#member-functions)

## Header

`/Test/TTL/container_wrapper.hlsli`

## Declaration

```c++
template<typename ContainerType, typename = void>
struct container_wrapper;
```

## Template Parameters

1. `ContainerType` -> The type that the `ttl::container_wrapper` is wrapping.
2. `typename = void` -> placeholder to provide a way of implementing [SFINAE](https://www.cppstories.com/2016/02/notes-on-c-sfinae/) for any specializations

## Member Types

| Member Type | Definition |
|-------------|------------|
|`underlying_type`| `ContainerType`|
| `element_type` | `typename container_traits<underlying_type>::element_type`|

## Member Constants

1. `static const bool writable = container_traits<underlying_type>::is_writable`

## Member Variables

1. `underlying_type Data`

## Member Functions

| Function | Description |
|---------------|-------------|
| `uint size()` | Returns the number of elements in the container |
| `uint size_in_bytes()` | Returns the size of the container in bytes |
| `element_type load(uint Index)` | Returns the element from the container that is at the index specified by `Index` |
| `void store(uint Index, element_type Item)` | Store the `Item` at position `Index` in the container. NOTE: Not implemented when `writable` is false |
| `void store(uint Index, element_type Item1, element_type Item2)` | Stores the items, `Item1` and `Item2`, starting at position `Index` in the container. NOTE: Not implemented when `writable` is false |
| `void store(uint Index, element_type Item1, element_type Item2, element_type Item3)` | Stores the items, `Item1`, `Item2` and `Item3`, starting at position `Index` in the container. NOTE: Not implemented when `writable` is false |
| `void store(uint Index, element_type Item1, element_type Item2, element_type Item3, element_type Item4)` | Stores the items, `Item1`, `Item2`, `Item3` and `Item4`, starting at position `Index` in the container. NOTE: Not implemented when `writable` is false |

---

[Top](#ttlcontainer_wrapper)