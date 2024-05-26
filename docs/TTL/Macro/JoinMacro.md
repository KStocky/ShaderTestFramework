[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Macro](./MacroHeader.md)

# TTL_JOIN_MACRO

Macro for taking two preprocessor tokens and combining them into a new token which is expected to result in a function-like macro. It then invokes this macro with the provided arguments.

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Example](#example)

## Header

`/Test/TTL/macro.hlsli`

## Declaration

```c++
#define TTL_JOIN_MACRO(InStem, InSuffix, ...)
```

## Example

```c++
#include "/Test/TTL/macro.hlsli"

#define DO_1(InVal) InVal * 2
#define DO_2(InVal) InVal * 10

#define DO(InVersion, InVal) TTL_JOIN_MACRO(DO_, InVersion, InVal)

static const int first = DO(1, 6);
static const int second = DO(2, 6);

_Static_assert(12 == first, "12 because it combines DO_ and 1 to make the DO_1 macro which doubles the supplied argument");
_Static_assert(60 == second, "60 because it combines DO_ and 2 to make the DO_2 macro which multiplies the supplied argument by 10");
```
---

[Top](#ttl_join_macro)