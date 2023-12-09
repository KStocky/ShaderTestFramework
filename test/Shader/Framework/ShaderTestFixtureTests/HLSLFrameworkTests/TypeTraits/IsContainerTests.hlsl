#include "/Test/Public/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_BuiltInArray_WHEN_IsContainerQueried_THEN_True()
{
    STF::IsTrue(STF::is_container<int[6]>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_NonArray_WHEN_IsContainerQueried_THEN_False()
{
    STF::IsFalse(STF::is_container<int>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_VectorType_WHEN_IsContainerQueried_THEN_False()
{
    STF::IsFalse(STF::is_container<int2>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_Buffer_WHEN_IsContainerQueried_THEN_True()
{
    STF::IsTrue(STF::is_container<Buffer<int> >::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_RWBuffer_WHEN_IsContainerQueried_THEN_True()
{
    STF::IsTrue(STF::is_container<RWBuffer<int> >::value);
}

struct MyStruct
{
    int a;
    float b;
};

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_StructuredBuffer_WHEN_IsContainerQueried_THEN_True()
{
    STF::IsTrue(STF::is_container<StructuredBuffer<MyStruct> >::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_RWStructuredBuffer_WHEN_IsContainerQueried_THEN_True()
{
    STF::IsTrue(STF::is_container<RWStructuredBuffer<MyStruct> >::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ByteAddressBuffer_WHEN_IsContainerQueried_THEN_True()
{
    STF::IsTrue(STF::is_container<ByteAddressBuffer>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_RWByteAddressBuffer_WHEN_IsContainerQueried_THEN_True()
{
    STF::IsTrue(STF::is_container<RWByteAddressBuffer>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ContainerInstantiation_WHEN_IsContainerQueried_THEN_True()
{
    STF::IsTrue(STF::is_container<STF::container<int[2]> >::value);
}