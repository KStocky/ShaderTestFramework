#include "/Test/Public/ShaderTestFramework.hlsli"

template<typename T1, typename T2>
void ContainerAsserts()
{
    STF::AreEqual(T1::is_container, T2::is_container);
    STF::AreEqual(T1::is_writable, T2::is_writable);
    STF::AreEqual(T1::is_byte_address, T2::is_byte_address);
    STF::AreEqual(T1::is_structured, T2::is_structured);
    STF::AreEqual(T1::is_resource, T2::is_resource);
    STF::IsTrue(STF::is_same<typename T1::element_type, typename T2::element_type>::value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_BuiltInArray_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected()
{
    using type = int[6];
    using traits = STF::container_traits<type>;
    using containerType = STF::container<type>;
    using containerTypeTraits = STF::container_traits<containerType>;

    STF::IsTrue(traits::is_container);
    STF::IsTrue(traits::is_writable);
    STF::IsFalse(traits::is_byte_address);
    STF::IsFalse(traits::is_structured);
    STF::IsFalse(traits::is_resource);
    STF::IsTrue(STF::is_same<traits::element_type, int>::value);

    ContainerAsserts<traits, containerTypeTraits>();
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_NonArray_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected()
{
    using type = int;
    using traits = STF::container_traits<type>;
    using containerType = STF::container<type>;
    using containerTypeTraits = STF::container_traits<containerType>;

    STF::IsFalse(traits::is_container);
    STF::IsFalse(traits::is_writable);
    STF::IsFalse(traits::is_byte_address);
    STF::IsFalse(traits::is_structured);
    STF::IsFalse(traits::is_resource);
    STF::IsTrue(STF::is_same<traits::element_type, void>::value);

    ContainerAsserts<traits, containerTypeTraits>();
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_VectorType_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected()
{
    using type = int2;
    using traits = STF::container_traits<type>;
    using containerType = STF::container<type>;
    using containerTypeTraits = STF::container_traits<containerType>;

    STF::IsFalse(traits::is_container);
    STF::IsFalse(traits::is_writable);
    STF::IsFalse(traits::is_byte_address);
    STF::IsFalse(traits::is_structured);
    STF::IsFalse(traits::is_resource);
    STF::IsTrue(STF::is_same<traits::element_type, void>::value);

    ContainerAsserts<traits, containerTypeTraits>();
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_Buffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected()
{
    using type = Buffer<int>;
    using traits = STF::container_traits<type>;
    using containerType = STF::container<type>;
    using containerTypeTraits = STF::container_traits<containerType>;

    STF::IsTrue(traits::is_container);
    STF::IsFalse(traits::is_writable);
    STF::IsFalse(traits::is_byte_address);
    STF::IsFalse(traits::is_structured);
    STF::IsTrue(traits::is_resource);
    STF::IsTrue(STF::is_same<traits::element_type, int>::value);

    ContainerAsserts<traits, containerTypeTraits>();
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_RWBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected()
{
    using type = RWBuffer<int>;
    using traits = STF::container_traits<type>;
    using containerType = STF::container<type>;
    using containerTypeTraits = STF::container_traits<containerType>;

    STF::IsTrue(traits::is_container);
    STF::IsTrue(traits::is_writable);
    STF::IsFalse(traits::is_byte_address);
    STF::IsFalse(traits::is_structured);
    STF::IsTrue(traits::is_resource);
    STF::IsTrue(STF::is_same<traits::element_type, int>::value);

    ContainerAsserts<traits, containerTypeTraits>();
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
    using traits = STF::container_traits<type>;
    using containerType = STF::container<type>;
    using containerTypeTraits = STF::container_traits<containerType>;

    STF::IsTrue(traits::is_container);
    STF::IsFalse(traits::is_writable);
    STF::IsFalse(traits::is_byte_address);
    STF::IsTrue(traits::is_structured);
    STF::IsTrue(traits::is_resource);
    STF::IsTrue(STF::is_same<traits::element_type, MyStruct>::value);

    ContainerAsserts<traits, containerTypeTraits>();
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_RWStructuredBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected()
{
    using type = RWStructuredBuffer<MyStruct>;
    using traits = STF::container_traits<type>;
    using containerType = STF::container<type>;
    using containerTypeTraits = STF::container_traits<containerType>;

    STF::IsTrue(traits::is_container);
    STF::IsTrue(traits::is_writable);
    STF::IsFalse(traits::is_byte_address);
    STF::IsTrue(traits::is_structured);
    STF::IsTrue(traits::is_resource);
    STF::IsTrue(STF::is_same<traits::element_type, MyStruct>::value);

    ContainerAsserts<traits, containerTypeTraits>();
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ByteAddressBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected()
{
    using type = ByteAddressBuffer;
    using traits = STF::container_traits<type>;
    using containerType = STF::container<type>;
    using containerTypeTraits = STF::container_traits<containerType>;

    STF::IsTrue(traits::is_container);
    STF::IsFalse(traits::is_writable);
    STF::IsTrue(traits::is_byte_address);
    STF::IsFalse(traits::is_structured);
    STF::IsTrue(traits::is_resource);
    STF::IsTrue(STF::is_same<traits::element_type, uint>::value);

    ContainerAsserts<traits, containerTypeTraits>();
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_RWByteAddressBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected()
{
    using type = RWByteAddressBuffer;
    using traits = STF::container_traits<type>;
    using containerType = STF::container<type>;
    using containerTypeTraits = STF::container_traits<containerType>;

    STF::IsTrue(traits::is_container);
    STF::IsTrue(traits::is_writable);
    STF::IsTrue(traits::is_byte_address);
    STF::IsFalse(traits::is_structured);
    STF::IsTrue(traits::is_resource);
    STF::IsTrue(STF::is_same<traits::element_type, uint>::value);

    ContainerAsserts<traits, containerTypeTraits>();
}
