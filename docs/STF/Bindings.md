[Reference](../ShaderTestFramework.md)

# Bindings

**Contents**
1. [Constant Buffer Bindings](#value-bindings)
2. [Resource Bindings](#resource-bindings)

## Value Bindings

All code snippets for this section will be taken from ([Ex8_ConstantBuffers](../../examples/Ex8_ConstantBuffers))

Shader Test Framework provides a simple way of specifying constant buffer bindings. i.e. bindings which are not buffers, samples or textures. `stf::ShaderTestFixture::RuntimeTestDesc` contains a member called `Bindings` which can be populated with a `std::vector` of pairs of `std::string`s and constant buffer data. Bindings must refer to global names. Meaning that you can't bind individual members of global parameters or constant buffers. You must provide all of the data to bind to a global name in the shader.

Given the following HLSL test:

```c++
struct MyStruct
{
    float3 Vec3;
    int2 IntPoint;
};

MyStruct GlobalBinding;

ConstantBuffer<MyStruct> CBuffer;

namespace MyNamespace
{
    int GlobalParam;
}

[numthreads(1, 1, 1)]
void TestWithConstantBuffers()
{
    ASSERT(AreEqual, GlobalBinding.IntPoint.x, 42);
    ASSERT(AreEqual, CBuffer.Vec3.z, 2.0);
    ASSERT(AreEqual, MyNamespace::GlobalParam, -1);
}
```

we can bind data to the following parameters in this shader:
1. `GlobalBinding`
2. `CBuffer`
3. `MyNamespace::GlobalParam`

However, we can not bind just `GlobalBinding.Vec3`, because that is a subobject of a global parameter. We must bind the entire global.

On the C++ side of things we can define the following struct:

```c++
struct StructUsedInHLSL
{
    stf::float3 Vector;
    float Padding;
    stf::int2 Point;
};
```

and then bind data to `GlobalBinding` with the following call to `stf::ShaderTestFixture::RunTest`:

```c++
fixture.RunTest(
        stf::ShaderTestFixture::RuntimeTestDesc
        {
            // Some runtime desc args

            .Bindings
            {
                {"GlobalBinding", StructUsedInHLSL{.Vector{0.0, 0.0, 0.0}, .Point{42, 24}}},
            },
            
            // Some more args
        })
```

Notice how `StructUsedInHLSL` has an extra member when compared to the HLSL equivilent `MyStruct`. We need to add 4 bytes of padding in between the `float3` and `int2` on the C++ side due to how constant buffer data is packed. The rules on how constant buffers are packed can be read [here](https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-packing-rules).

Then when we set data on `GlobalBinding` we simply provide an instantiation of `StructUsedInHLSL`. [Designated Initializers](https://en.cppreference.com/w/cpp/language/aggregate_initialization#Designated_initializers) work well for this because you can specify just the members you care about initializing and ignore any members that you don't care about (e.g. we don't care what data is in the padding bytes)

## Resource Bindings

Currently STF does not support binding (or creating) resources.

---

[Top](#bindings)