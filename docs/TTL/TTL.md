[Reference](../ShaderTestFramework.md)

# Test Template Library

The Test Template Library (TTL) is analogous to the [C++ standard library](https://en.cppreference.com/w/cpp/header). It contains a collection of headers which are not dependent on the Shader Test Framework. It is a standalone library of templates and functions. It does depend on the following compiler flags to compile:

- -HV 2021
- -enable-16bit-types

## HLSL and Library features

- [Pseudo-concepts](./PseudoConcepts.md)

## Headers

| | |
|-|-|
| [`caster.hlsli`](./Caster.md) | specializable utility to define conversions between arbitrary types |
| [`concepts.hlsli`](./Concepts/ConceptsHeader.md) | Library of [pseudo-concepts](./PseudoConcepts.md) |
| [`container_wrapper.hlsli`](./ContainerWrapper.md) | class template for providing a unified interface around resource buffers and arrays |
| [`models.hlsli`](./Models/ModelsHeader.md) | Library for evaluating and constructing [pseudo-concepts](./PseudoConcepts.md) |
| [`static_assert.hlsli`](./StaticAssert.md) | Macro for supply a `static_assert` like utility |
| [`type_traits.hlsli`](./TypeTraits/TypeTraitsHeader.md) | Compile time type information |