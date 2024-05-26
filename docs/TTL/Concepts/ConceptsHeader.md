[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md)

# Concepts

The TTL (Test Template Library) provides a number of [Pseudo-concepts](../PseudoConcepts.md) in the [`concepts.hlsli`](../../../src/ShaderTestFramework/Shader/TTL/concepts.hlsli) header. These operate in a similar way to [C++20 concepts](https://en.cppreference.com/w/cpp/header/concepts) when used with [`ttl::models`](../Models/Models.md) and other meta function is the [models library](../Models/ModelsHeader.md).


## Language concepts

| | |
|-|-|
|[`string_literal`](./StringLiteral.md) | Specifies that a type is a string literal |

## Comparison concepts

| | |
|---------|-------------|
|[`equality_comparable`<br>`equality_comparable_with`](./EqualityComparable.md) | Specifies that `operator==` and `operator!=` are defined |

## Callable concepts

| | |
|-|-|
|[`invocable_functor`<br>`invocable`](Invocable.md) | Specifies that callables can be invoked wiht a given set of argument types |



