#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_BuiltInArray_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected()
{
    using type = int[6];
    using traits = ttl::container_traits<type>;

    STF::IsTrue(traits::is_container);
    STF::IsTrue(traits::is_writable);
    STF::IsFalse(traits::is_byte_address);
    STF::IsFalse(traits::is_structured);
    STF::IsFalse(traits::is_resource);
    STF::IsTrue(ttl::is_same<traits::element_type, int>::value);
    STF::IsTrue(ttl::is_same<traits::type, type>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_NonArray_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected()
{
    using type = int;
    using traits = ttl::container_traits<type>;

    STF::IsFalse(traits::is_container);
    STF::IsFalse(traits::is_writable);
    STF::IsFalse(traits::is_byte_address);
    STF::IsFalse(traits::is_structured);
    STF::IsFalse(traits::is_resource);
    STF::IsTrue(ttl::is_same<traits::element_type, void>::value);
    STF::IsTrue(ttl::is_same<traits::type, void>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_VectorType_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected()
{
    using type = int2;
    using traits = ttl::container_traits<type>;

    STF::IsFalse(traits::is_container);
    STF::IsFalse(traits::is_writable);
    STF::IsFalse(traits::is_byte_address);
    STF::IsFalse(traits::is_structured);
    STF::IsFalse(traits::is_resource);
    STF::IsTrue(ttl::is_same<traits::element_type, void>::value);
    STF::IsTrue(ttl::is_same<traits::type, void>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_Buffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected()
{
    using type = Buffer<int>;
    using traits = ttl::container_traits<type>;

    STF::IsTrue(traits::is_container);
    STF::IsFalse(traits::is_writable);
    STF::IsFalse(traits::is_byte_address);
    STF::IsFalse(traits::is_structured);
    STF::IsTrue(traits::is_resource);
    STF::IsTrue(ttl::is_same<traits::element_type, int>::value);
    STF::IsTrue(ttl::is_same<traits::type, type>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_RWBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected()
{
    using type = RWBuffer<int>;
    using traits = ttl::container_traits<type>;
    STF::IsTrue(traits::is_container);
    STF::IsTrue(traits::is_writable);
    STF::IsFalse(traits::is_byte_address);
    STF::IsFalse(traits::is_structured);
    STF::IsTrue(traits::is_resource);
    STF::IsTrue(ttl::is_same<traits::element_type, int>::value);
    STF::IsTrue(ttl::is_same<traits::type, type>::value);
}

struct MyStruct
{
    int a;
    float b;
};

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_StructuredBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected()
{
    using type = StructuredBuffer<MyStruct>;
    using traits = ttl::container_traits<type>;

    STF::IsTrue(traits::is_container);
    STF::IsFalse(traits::is_writable);
    STF::IsFalse(traits::is_byte_address);
    STF::IsTrue(traits::is_structured);
    STF::IsTrue(traits::is_resource);
    STF::IsTrue(ttl::is_same<traits::element_type, MyStruct>::value);
    STF::IsTrue(ttl::is_same<traits::type, type>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_RWStructuredBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected()
{
    using type = RWStructuredBuffer<MyStruct>;
    using traits = ttl::container_traits<type>;

    STF::IsTrue(traits::is_container);
    STF::IsTrue(traits::is_writable);
    STF::IsFalse(traits::is_byte_address);
    STF::IsTrue(traits::is_structured);
    STF::IsTrue(traits::is_resource);
    STF::IsTrue(ttl::is_same<traits::element_type, MyStruct>::value);
    STF::IsTrue(ttl::is_same<traits::type, type>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ByteAddressBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected()
{
    using type = ByteAddressBuffer;
    using traits = ttl::container_traits<type>;

    STF::IsTrue(traits::is_container);
    STF::IsFalse(traits::is_writable);
    STF::IsTrue(traits::is_byte_address);
    STF::IsFalse(traits::is_structured);
    STF::IsTrue(traits::is_resource);
    STF::IsTrue(ttl::is_same<traits::element_type, uint>::value);
    STF::IsTrue(ttl::is_same<traits::type, type>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_RWByteAddressBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected()
{
    using type = RWByteAddressBuffer;
    using traits = ttl::container_traits<type>;

    STF::IsTrue(traits::is_container);
    STF::IsTrue(traits::is_writable);
    STF::IsTrue(traits::is_byte_address);
    STF::IsFalse(traits::is_structured);
    STF::IsTrue(traits::is_resource);
    STF::IsTrue(ttl::is_same<traits::element_type, uint>::value);
    STF::IsTrue(ttl::is_same<traits::type, type>::value);
}
