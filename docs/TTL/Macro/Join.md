[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Macro](./MacroHeader.md)

# TTL_JOIN

Macro for taking two preprocessor tokens and combining them into a new token.

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Example](#example)

## Header

`/Test/TTL/macro.hlsli`

## Declaration

```c++
#define TTL_JOIN(InA, InB)
```

## Example

```c++

#include "/Test/TTL/macro.hlsli"

#define UNIQUE_VAR(InName, InId) TTL_JOIN(InName, InId)

static const int UNIQUE_VAR(myVar, 0) = 42;
static const int UNIQUE_VAR(myVar, 1) = 34;

_Static_assert(42 == myVar0, "");
_Static_assert(34 == myVar1, "");


```
---

[Top](#ttl_join)