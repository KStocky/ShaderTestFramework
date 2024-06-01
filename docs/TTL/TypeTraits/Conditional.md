[Reference](../../ShaderTestFramework.md) -> [Test Template Library](../TTL.md) -> [Type Traits](./TypeTraitsHeader.md)

# ttl::conditional

Provides member type alias, which is defined as `T1` if `InCond` is `true`, or as `T2` if `InCond` is `false`

If the program adds specializations for `ttl::conditional` or `ttl::conditional_t`, the behavior is undefined. 

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Helper variable template](#helper-variable-template)
4. [Template Parameters](#template-parameters)
5. [Member Types](#member-types)
6. [Example](#example)

## Header

`/Test/TTL/type_traits.hlsli`

## Declaration

```c++
template<bool InCond, typename T1, typename T2>
struct conditional
```

## Helper variable template

```c++
template<bool InCond, typename T1, typename T2>
using conditional_t = typename conditional<InCond, T1, T2>::type;
```

## Template Parameters

1. `InCond` -> Compile time boolean value that determines whether `type` is an alias for `T1` or `T2`
2. `T1` -> If `InCond` is `true`, `type` is an alias for this type
3. `T2` -> If `InCond` is `false`, `type` is an alias for this type

## Member Types

| Member type | Definition |
|-------------|------------|
| `type`      | `T1` if `InCond == true`, `T2` if `InCond == false` |



## Example

```c++

#include "/Test/TTL/type_traits.hlsli"

struct A{};
struct B{};

using TrueAB = ttl::conditional<true, A, B>::type;
using FalseAB = ttl::conditional<false, A, B>::type;

_Static_assert(ttl::is_same_v<TrueAB, A>, "TrueAB is a type alias for A because the provided bool was true");
_Static_assert(ttl::is_same_v<FalseAB, B>, "FalseAB is a type alias for B because the provided bool was false");

```
---

[Top](#ttlconditional)