[Reference](../ShaderTestFramework.md) -> [Test Template Library](./TTL.md)

# Static Assert

The constant boolean expression is evaluated at compile time. If it evalulates to `false`, a compile-time error occurs and if a display message is provided by the `_Static_assert` it is displayed as part of the compilation error.
Otherwise, if expression evaluates to `true`, nothing happens; no code is emitted.  This can be useful for writing tests for code that is known at compile time e.g. template meta programming.

**Contents**
1. [Syntax](#syntax)
2. [Example](#example)<br>

## Syntax

1. `_Static_assert(condition);` - If `condition` is `false`, compilation of the shader will fail at this statement. Otherwise, compilation will continue.
2. `_Static_assert(condition, string_literal_message);` - If `condition` is `false`, compilation of the shader will fail at this statement, and the message will be displayed in the shader compilation errors. Otherwise, compilation will continue.


## Example

```c++

template<typename T>
static const bool is_int = false;

template<>
static const bool is_int<int> = true;

_Static_assert(42 > 34);

_Static_assert(is_int<float>, "This will fail compilation and show this message because float is not an int");
_Static_assert(is_int<int>, "This will succeed compilation and be ignored");

```
---

[Top](#static-assert)