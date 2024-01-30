# Compile Time Tests

Sometimes we only want to write tests which check if a shader compiles or not. In these cases there is no need to actually dispatch the shader test. The very fact that it compiled is good enough for us. That is where Compile Time Tests come in. Throughout this topic, we will be referring to ([Ex5_CompileTimeTests](../examples/Ex5_CompileTimeTests))

**Contents**


## When Compile Time Tests are appropriate

The following are some cases where you might consider a compile time test over a standard test:

1. You are writing tests for some template meta programming code - In this case, all of your asserts could likely be performed by [STATIC_ASSERT](./StaticAssert.md). If your shader compiles and `STATIC_ASSERTS` are present in the shader, then that means all of the `STATIC_ASSERT`s passed

2. Testing out language features - Sometimes it can be beneficial to have tests for HLSL code just to see if they are valid HLSL. This is particularly useful if you want to provide minimal repro code to demonstrate a bug with DXC.

## Writing a Compile Time Test

Writing compile time tests are different from regular run time tests differs both on the C++ and the HLSL side of the test.

### C++

The following code from [CompileTimeTests.cpp](../examples/Ex5_CompileTimeTests/CompileTimeTests.cpp) demonstrates how to dispatch a compile time test:

```c++
#include <Framework/ShaderTestFixture.h>

#include <catch2/catch_test_macros.hpp>

SCENARIO("Example5Tests")
{
    ShaderTestFixture::Desc desc{};
    desc.Mappings.emplace_back(VirtualShaderDirectoryMapping{"/Shader", std::filesystem::current_path() / SHADER_SRC});
    desc.Source = std::filesystem::path{ "/Shader/ConditionalTypeTests.hlsl" };

    ShaderTestFixture fixture(std::move(desc));
    
    REQUIRE(fixture.RunCompileTimeTest("AllCompile"));
}
```

The main difference is that we are calling `ShaderTestFixture::RunCompileTimeTest` to run the test. This differs from `ShaderTestFixture::RunTest` in that it does not take a dispatch configuration. This is because the shader will not be executed.

### HLSL

In this example we will be writing tests for `ConditionalType` which is essentially an implementation of [`std::conditional`](https://en.cppreference.com/w/cpp/types/conditional). This type trait takes 3 template arguments.

1. `bool Condition` - The condition passed.
2. `typename IfTrue` - The type returned in `Type` when the `Condition` is `true`.
3. `typename IfFalse` - The type returned in `Type` when the `Condition` is `false`. 

The implementation can be found in [ConditionalType.hlsli](../examples/Ex5_CompileTimeTests/ShaderCode/ConditionalType.hlsli). To test this type trait we might want to write a couple of asserts to make sure that the expected type is returned when the condition is true and false. The following is how we might do that.

```c++
#include "/Shader/ConditionalType.hlsli"
#include "/Test/TTL/static_assert.hlsli"

STATIC_ASSERT((ttl::is_same<int, ConditionalType<true, int, float>::Type>::value));
STATIC_ASSERT((ttl::is_same<float, ConditionalType<false, int, float>::Type>::value));

[numthreads(1, 1, 1)]
void AllCompile()
{
}
```

Here we are not including the full testing framework as we do not need it. We just need the `STATIC_ASSERT` header. And we are just putting our `STATIC_ASSERT`s in the global namespace. `STATIC_ASSERT`s can appear anywhere a declaration can. They are not confined to being written in functions like `ASSERT`. Each `STATIC_ASSERT` has its condition surrounded by an extra set of parentheses due to the fact that it is a macro and we have commas in our condition (`ttl::is_same<T,T>::value`). The only reason we have a function defined here is because we need an entry function to be defined to compile


The main difference on the HLSL side is that we are using `STATIC_ASSERT` instead of `ASSERT`. More information on `STATIC_ASSERT` can be found [here](./StaticAssert.md). 