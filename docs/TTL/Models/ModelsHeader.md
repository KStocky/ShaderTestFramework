[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md)

# Models

The TTL provides [`ttl::models`](./Models.md) as well as other utility meta-functions as a way of constructing and evaluating [pseudo-concepts](../PseudoConcepts.md).

## Evaluation

| | |
|-|-|
| [`models`](./Models.md) | Evaluates a [pseudo-concepts](../PseudoConcepts.md). `true` if the pseudo-concept is satisfied, `false` otherwise


## Construction Helpers

| | |
|-|-|
| [`models_if`](./ModelsIf.md) | meta-function for satisfying a constrait in a pseudo-concept if the provided `bool` evaluates to `true`, fails otherwise |
| [`models_if_pred`](./ModelsIfPred.md) | meta-function for satisfying a constrait in a pseudo-concept if the provided template template argument evaluates to `true` when the supplied type is provided, fails otherwise |
| [`models_if_same`](./ModelsIfSame.md) | meta-function for satisfying a constraint in a pseudo-concept if the types, `T` and `U` are the same, fails otherwise |
| [`models_refines`](./ModelsRefines.md) | meta-function for composing one pseudo-concept with another |