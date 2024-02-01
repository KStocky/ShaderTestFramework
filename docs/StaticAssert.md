# Static Assert

Shader Test Framework provides a static assert facility that attempts to emulate [static_assert](https://en.cppreference.com/w/cpp/language/static_assert) from C++. This can be useful for writing tests for code that is known at compile time e.g. template meta programming.

**Contents**
1. [Header](#header)
2. [Syntax](#syntax)
3. [Notes](#notes)<br>
    a. [1. Where `STATIC_ASSERT` can be used](#1-where-static_assert-can-be-used)<br>
    b. [2. Conditions with commas](#2-conditions-with-commas)<br>
    c. [3. ttl::always_false<T>](#3-ttlalways_false)
4. [Examples](#examples)<br>

## Header

`/Test/TTL/static_assert.hlsli`

## Syntax

1. `STATIC_ASSERT(condition);` - If `condition` is false, compilation of the shader will fail at this statement. Otherwise, compilation will continue.
2. `STATIC_ASSERT(condition, string_literal_message);` - If `condition` is false, compilation of the shader will fail at this statement, and the message will be displayed in the shader compilation errors. Otherwise, compilation will continue.

## Notes

### 1. Where `STATIC_ASSERT` can be used
`STATIC_ASSERT` can be used in every context that [static_assert](https://en.cppreference.com/w/cpp/language/static_assert) can. Some of these contexts are listed below:

1. Global/Namespace scope
2. In structs
3. In Functions

### 2. Conditions with commas

Since `STATIC_ASSERT` is implemented as a macro, every comma is interpretted as a macro argument delimiter. A common occurance of having a comma in your condition is if the condtion is using a type trait like `ttl::is_same<T, U>`. To avoid compilation errors due to this surround the condition with parentheses.

### 3. ttl::always_false<T>

Just like with [static_assert](https://en.cppreference.com/w/cpp/language/static_assert), `STATIC_ASSERT(false)` within a template will always fail to compile regardless of if the template has been instantiated or not. This issue is discussed in [Allowing static_assert(false)](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2593r0.html). To get around this issue, `ttl::always_false<T>` can be used to ensure that if a templated is instantiated that compilation will fail.

## Examples

1. Without a string message

```c++
#include "/Test/TTL/static_assert.hlsli"

// This will pass
STATIC_ASSERT(42 > 34);

```

2. Failing without a string message

```c++
#include "/Test/TTL/static_assert.hlsli"

// This will fail
STATIC_ASSERT(42 < 34);

```
This will fail with the following error message

```
Expanded: `hlsl.hlsl:5:1: error: invalid value, valid range is between 1 and 4 inclusive
STATIC_ASSERT(42 < 34);
^
.//Test/TTL/static_assert.hlsli:7:65: note: expanded from macro 'STATIC_ASSERT'
#define STATIC_ASSERT(Expression, ...) static const vector<int, !!(Expression)> CONCAT_STATIC_ASSERT(ASSERT_VAR_, __COUNTER__)
```

3. Failing with a string message

```c++
#include "/Test/TTL/static_assert.hlsli"

// This will fail
STATIC_ASSERT(42 < 34, "Oops");

```

This will fail with the following message

```
`hlsl.hlsl:5:1: error: invalid value, valid range is between 1 and 4 inclusive
STATIC_ASSERT(42 < 34, "Oops");
^
.//Test/TTL/static_assert.hlsli:7:65: note: expanded from macro 'STATIC_ASSERT'
#define STATIC_ASSERT(Expression, ...) static const vector<int, !!(Expression)> CONCAT_STATIC_ASSERT(ASSERT_VAR_, __COUNTER__)
```

4. Condition with a comma

```c++
#include "/Test/TTL/static_assert.hlsli"
#include "/Test/TTL/type_traits.hlsli"

// Note the extra parentheses around the condition
STATIC_ASSERT((ttl::is_same<int, int>::value));

```

4. Static assert when a particular template is instantiated

```c++
#include "/Test/TTL/static_assert.hlsli"
#include "/Test/TTL/type_traits.hlsli"

template<typename T>
struct OnlyValidForFloat
{
    // To ensure that this will only be activated if this template is instantiated we must use ttl::always_false
    // Otherwise, this will fail compilation even if it is never instantiated.
    STATIC_ASSERT(ttl::always_false<T>::value, "Invalid use of this template! You should only instantiate it with a float");
};

template<>
struct OnlyValidForFloat<float>
{

};

```

---

[Top](#static-assert)