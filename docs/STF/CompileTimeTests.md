[Reference](../ShaderTestFramework.md)

# Compile Time Tests

Sometimes we only want to write tests which check if a shader compiles or not. In these cases there is no need to actually dispatch the shader test. The very fact that it compiled is good enough for us. That is where Compile Time Tests come in. Throughout this topic, we will be referring to ([Ex5_CompileTimeTests](../../examples/Ex5_CompileTimeTests))

**Contents**

1. [When Compile Time Tests are appropriate](#when-compile-time-tests-are-appropriate)
2. [Writing a Compile Time Test](#writing-a-compile-time-test)<br>
    a. [C++](#c)<br>
    b. [HLSL](#hlsl)
3. [Providing Error Messages](#providing-error-messages)


## When Compile Time Tests are appropriate

The following are some cases where you might consider a compile time test over a standard test:

1. You are writing tests for some template meta programming code - In this case, all of your asserts could likely be performed by [STATIC_ASSERT](../TTL/StaticAssert.md). If your shader compiles and `STATIC_ASSERTS` are present in the shader, then that means all of the `STATIC_ASSERT`s passed

2. Testing out language features - Sometimes it can be beneficial to have tests for HLSL code just to see if they are valid HLSL. This is particularly useful if you want to provide minimal repro code to demonstrate a bug with DXC.

## Writing a Compile Time Test

Writing compile time tests are different from regular run time tests differs both on the C++ and the HLSL side of the test.

### C++

The following code from [CompileTimeTests.cpp](../../examples/Ex5_CompileTimeTests/CompileTimeTests.cpp) demonstrates how to run a compile time test:

```c++
#include <Framework/ShaderTestFixture.h>

#include <catch2/catch_test_macros.hpp>

SCENARIO("Example5Tests")
{
    ShaderTestFixture::Desc desc{};
    desc.Mappings.emplace_back(VirtualShaderDirectoryMapping{"/Shader", std::filesystem::current_path() / SHADER_SRC});
    desc.Source = std::filesystem::path{ "/Shader/ConditionalTypeTests.hlsl" };

    ShaderTestFixture fixture(std::move(desc));
    
    REQUIRE(fixture.RunCompileTimeTest());
}
```

The main difference is that we are calling `ShaderTestFixture::RunCompileTimeTest` to run the test. Compile Time Tests are not executed. Therefore an entry function name and a dispatch configuration is not required.

### HLSL

In this example we will be writing tests for `ConditionalType` which is essentially an implementation of [`std::conditional`](https://en.cppreference.com/w/cpp/types/conditional). It implemented as follows:
```c++
template<bool InCond, typename IfTrue, typename IfFalse>
struct ConditionalType
{
    using Type = IfTrue;
};

template<typename IfTrue, typename IfFalse>
struct ConditionalType<false, IfTrue, IfFalse>
{
    using Type = IfFalse;
};
```

This type trait takes 3 template arguments.

1. `bool Condition` - The condition passed.
2. `typename IfTrue` - The type returned in `Type` when the `Condition` is `true`.
3. `typename IfFalse` - The type returned in `Type` when the `Condition` is `false`. 

The implementation can be found in [ConditionalType.hlsli](../../examples/Ex5_CompileTimeTests/ShaderCode/ConditionalType.hlsli). To test this type trait we might want to write a couple of asserts to make sure that the expected type is returned when the condition is true and false. The following is how we might do that.

```c++
#include "/Shader/ConditionalType.hlsli"
#include "/Test/TTL/static_assert.hlsli"

STATIC_ASSERT((ttl::is_same<int, ConditionalType<true, int, float>::Type>::value));
STATIC_ASSERT((ttl::is_same<float, ConditionalType<false, int, float>::Type>::value));

```

Here we are not including the full testing framework as we do not need it. We just need the `STATIC_ASSERT` header. And we are just putting our `STATIC_ASSERT`s in the global namespace. `STATIC_ASSERT`s can appear anywhere a declaration can. They are not confined to being written in functions like `ASSERT`. Each `STATIC_ASSERT` has its condition surrounded by an extra set of parentheses due to the fact that it is a macro and we have commas in our condition (`ttl::is_same<T,T>::value`). As mentioned in the [C++](#c) section, there is no entry function required.

## Providing Error Messages

If for the sake of argument we were to change the first `STATIC_ASSERT` of the previous example to:
```c++
STATIC_ASSERT((ttl::is_same<float, ConditionalType<true, int, float>::Type>::value));
```

We would see the following failure.

```
invalid value, valid range is between 1 and 4
  inclusive
  STATIC_ASSERT((ttl::is_same<float, ConditionalType<true, int, float>::Type>::
  value));
```

There is some irrelevant text at the start (`invalid value, valid range is between 1 and 4 inclusive`). This is due to the implementation of `STATIC_ASSERT`. And then it gives us the condition that actually failed. Which is great. However, sometimes the condition might not be very readable, and may make it hard to understand what went wrong. To improve this situation the `STATIC_ASSERT` macro can take a second parameter which is a string message. An example of this is could be the following:

```c++
STATIC_ASSERT((ttl::is_same<int, ConditionalType<true, int, float>::Type>::value), "Expected the first type to be returned when the condition is true");
STATIC_ASSERT((ttl::is_same<float, ConditionalType<false, int, float>::Type>::value), "Expected the second type to be returned when the condition is false");
```

Now if we change the first `STATIC_ASSERT` to fail, we will get the following output:

```
invalid value, valid range is between 1 and 4
  inclusive
  STATIC_ASSERT((ttl::is_same<float, ConditionalType<true, int, float>::Type>::
  value), "Expected the first type to be returned when the condition is true");
```

It still has the same irrelevant text at the start but now we also get some human readable text to tell us what went wrong.

---

[Top](#compile-time-tests)