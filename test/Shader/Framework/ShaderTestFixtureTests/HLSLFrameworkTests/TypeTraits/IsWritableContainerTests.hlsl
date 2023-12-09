#include "/Test/Public/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_BuiltInArray_WHEN_IsWritableContainerQueried_THEN_True()
{
    STF::IsTrue(STF::is_writable_container<int[6]>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_NonArray_WHEN_IsWritableContainerQueried_THEN_False()
{
    STF::IsFalse(STF::is_writable_container<int>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_VectorType_WHEN_IsWritableContainerQueried_THEN_False()
{
    STF::IsFalse(STF::is_writable_container<int2>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_Buffer_WHEN_IsWritableContainerQueried_THEN_False()
{
    STF::IsFalse(STF::is_writable_container<Buffer<int> >::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_RWBuffer_WHEN_IsWritableContainerQueried_THEN_True()
{
    STF::IsTrue(STF::is_writable_container<RWBuffer<int> >::value);
}

struct MyStruct
{
    int a;
    float b;
};

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_StructuredBuffer_WHEN_IsWritableContainerQueried_THEN_False()
{
    STF::IsFalse(STF::is_writable_container<StructuredBuffer<MyStruct> >::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_RWStructuredBuffer_WHEN_IsWritableContainerQueried_THEN_True()
{
    STF::IsTrue(STF::is_writable_container<RWStructuredBuffer<MyStruct> >::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ByteAddressBuffer_WHEN_IsWritableContainerQueried_THEN_False()
{
    STF::IsFalse(STF::is_writable_container<ByteAddressBuffer>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_RWByteAddressBuffer_WHEN_IsWritableContainerQueried_THEN_True()
{
    STF::IsTrue(STF::is_writable_container<RWByteAddressBuffer>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_WritableContainerInstantiation_WHEN_IsWritableContainerQueried_THEN_True()
{
    STF::IsTrue(STF::is_writable_container<STF::container<int[2]> >::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_NonWritableContainerInstantiation_WHEN_IsWritableContainerQueried_THEN_False()
{
    STF::IsFalse(STF::is_writable_container<STF::container<Buffer<int> > >::value);
}