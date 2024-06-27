[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Macro](./MacroHeader.md)

# TTL_STAMP

Macro for stamping out `n` instances of the supplied function-like macro `x`. `x` is an initial function like macro that kicks off the stamp.

NOTE: `n` can either be `4`, `16`, `64` or `256`. Any other number is not supported, and will produce a compilation error. 

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Parameters](#parameters)
4. [Parameters of `x`](#parameters-of-x)
5. [Parameters of `InStamper`](#parameters-of-instamper)
6. [Parameters of `InStamp`](#parameters-of-instamp)
7. [Example](#example)

## Header

`/Test/TTL/macro.hlsli`

## Declaration

```c++
TTL_STAMP(n, x, ...)
```

## Parameters

| Parameter | Description |
|-----------|-------------|
| `n`       | The number of times to stamp out `x`. This can either be `4`, `16`, `64` or `256`. Any other number is not supported |
| `x`       | The function-like macro to start the stamping. |
| `...`     | The arguments to pass to `x` |

## Parameters of x

This macro is defined by the user to kick off the stamping of another function like macro

| Parameter | Description |
|-----------|-------------|
| `InStamper` | The stamping out function |
| `InN`       | The number of times to stamp the instance out. |
| `...` | The additional arguments to pass to `InStamp` |

## Parameters of `InStamper`

This macro is defined by the library. It is only meant to be accepted by `x` and then invoked by `x`

| Parameter | Description |
|-----------|-------------|
| `InI`     | The starting number of iteration |
| `InStamp` | The macro to stamp out. |
| `...` | The additional arguments to pass to `InStamp` |

## Parameters of `InStamp`

This macro is defined by the user. This is the actual macro that will get stamped out `n` times.

| Parameter | Description |
|-----------|-------------|
| `InI`     | Current iteration of the stamp |
| `...`     | Additional arguments |

## Example

```c++

#include "/Test/TTL/macro.hlsli"

#define ADD_TO_VALUE(InN) value += 1;
#define STAMP_ADD(InStamper, InN) InStamper(1, ADD_TO_VALUE)
int value = 0;

TTL_STAMP(16, STAMP_ADD)

// At this point value is 16

#define ADD_NUM_TO_VAR(InN, InVar, InValue) InVar += InValue;
#define STAMP_ADD_NUM_TO_VAR(InStamper, InN, InVar, InValue) InStamper(1, ADD_NUM_TO_VAR, InVar, InValue)

int myOtherValue = 0;

TTL_STAMP(64, STAMP_ADD_NUM_TO_VAR, myOtherValue, 4)

// At this point myOtherVlaue is 256.

```
---

[Top](#ttl_stamp)