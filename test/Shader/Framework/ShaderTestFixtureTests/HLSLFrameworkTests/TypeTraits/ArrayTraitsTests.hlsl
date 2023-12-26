#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_BuiltInArray_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    static const uint size = 6;
    using type = int[size];
    using traits = STF::array_traits<type>;

    STF::IsTrue(traits::is_array);
    STF::AreEqual(traits::size, size);
    STF::IsTrue(STF::is_same<traits::element_type, int>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_NonArray_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    using type = int;
    using traits = STF::array_traits<type>;

    STF::IsFalse(traits::is_array);
    STF::AreEqual(traits::size, 0u);
    STF::IsTrue(STF::is_same<traits::element_type, void>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_VectorType_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    using type = int2;
    using traits = STF::array_traits<type>;

    STF::IsFalse(traits::is_array);
    STF::AreEqual(traits::size, 0u);
    STF::IsTrue(STF::is_same<traits::element_type, void>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_Buffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    using type = Buffer<int>;
    using traits = STF::array_traits<type>;

    STF::IsFalse(traits::is_array);
    STF::AreEqual(traits::size, 0u);
    STF::IsTrue(STF::is_same<traits::element_type, void>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_RWBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    using type = RWBuffer<int>;
    using traits = STF::array_traits<type>;

    STF::IsFalse(traits::is_array);
    STF::AreEqual(traits::size, 0u);
    STF::IsTrue(STF::is_same<traits::element_type, void>::value);
}

struct MyStruct
{
    int a;
    float b;
};

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_StructuredBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    using type = StructuredBuffer<MyStruct>;
    using traits = STF::array_traits<type>;

    STF::IsFalse(traits::is_array);
    STF::AreEqual(traits::size, 0u);
    STF::IsTrue(STF::is_same<traits::element_type, void>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_RWStructuredBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    using type = RWStructuredBuffer<MyStruct>;
    using traits = STF::array_traits<type>;

    STF::IsFalse(traits::is_array);
    STF::AreEqual(traits::size, 0u);
    STF::IsTrue(STF::is_same<traits::element_type, void>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ByteAddressBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    using type = ByteAddressBuffer;
    using traits = STF::array_traits<type>;

    STF::IsFalse(traits::is_array);
    STF::AreEqual(traits::size, 0u);
    STF::IsTrue(STF::is_same<traits::element_type, void>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_RWByteAddressBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    using type = RWByteAddressBuffer;
    using traits = STF::array_traits<type>;

    STF::IsFalse(traits::is_array);
    STF::AreEqual(traits::size, 0u);
    STF::IsTrue(STF::is_same<traits::element_type, void>::value);
}
