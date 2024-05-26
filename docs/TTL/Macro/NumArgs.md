[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Macro](./MacroHeader.md)

# TTL_NUM_ARGS

Macro for counting the number of arguments provided to a macro. Useful for supporting macros that do different things depending on the number of arguments passed to it.

NOTE: Supports counting up to and including 63 arguments. Supplying 64 or more arguments is an error

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Example](#example)

## Header

`/Test/TTL/macro.hlsli`

## Declaration

```c++
#define TTL_NUM_ARGS(...)
```

## Example

```c++
#include "/Test/TTL/macro.hlsli"

static const int num = TTL_NUM_ARGS(a, b, c, d, e, f);

_Static_assert(6 == num, "");
```
---

[Top](#ttl_num_args)