[Reference](../ShaderTestFramework.md) -> [Test Template Library](./TTL.md)

# ttl::caster

Despite [Announcing HLSL 2021](https://devblogs.microsoft.com/directx/announcing-hlsl-2021/) claiming that conversion operators work in HLSL 2021, they do not. [Bug](https://github.com/microsoft/DirectXShaderCompiler/issues/6081). To workaround this, the Shader Test Framework provides `ttl::caster` as a templated struct which can be specialized to provide a conversion function. This provides a unified interface for doing user defined conversions for test writers.

**Contents**
1. [Header](#header)
2. [Declaration](#declaration)
3. [Template Parameters](#template-parameters)
4. [Member Constants](#member-constants)
5. [Member Functions](#member-functions)
6. [Non-Member Functions](#non-member-functions)
7. [Examples](#examples)

## Header

`/Test/TTL/caster.hlsli`

## Declaration

```c++
template<typename To, typename From, typename = void>
struct caster;
```

## Template Parameters

1. `To` -> The type of the output object.
2. `From` -> The type of the input object.
3. `typename = void` -> placeholder to provide a way of implementing [SFINAE](https://www.cppstories.com/2016/02/notes-on-c-sfinae/) for any specializations

## Member Constants

1. `static const bool writable = container_traits<underlying_type>::is_writable`

## Member Functions

| Function | Description |
|---------------|-------------|
| `static To cast(From)` | Take an object of type, `From` and returns an object of type `To` |

## Non-Member Functions

| Function | Description |
|---------------|-------------|
| `template<typename To, typename From> To cast(From In)` | Convenience function which removes the need of the caller to spell the type of the object that is being converted. |

## Examples

One of the more compelling reasons for providing a `ttl::caster` specialization for your type is to provide a cast to `bool`. This will allow test writers to use the `IsTrue` and `IsFalse` assertions with that data type. An example of this can be found in [TypeWithCastToBool.hlsl](../../examples/Ex7_Casters/ShaderCode/TypeWithCastToBool.hlsl). To run this example run the [Ex7_Casters](../../examples/Ex7_Casters/) example. The following is the relevant HLSL:

```c++
struct MyType
{
    uint a;
    float64_t b;
    bool c;
};

namespace ttl
{
    template<>
    struct caster<bool, MyType>
    {
        static bool cast(MyType In)
        {
            return In.a < 4;
        }
    };
}

[numthreads(1, 1, 1)]
void Test()
{
    MyType evalToTrue = (MyType)0;
    evalToTrue.a = 2;
    MyType evalToFalse = (MyType)0;
    evalToFalse.a = 5;

    ASSERT(IsTrue, evalToTrue);
    ASSERT(IsFalse, evalToFalse);
}
```
---

[Top](#ttlcaster)