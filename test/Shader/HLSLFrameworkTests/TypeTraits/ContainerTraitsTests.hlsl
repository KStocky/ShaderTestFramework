#include "/Test/TTL/static_assert.hlsli"
#include "/Test/TTL/type_traits.hlsli"

namespace GIVEN_BuiltInArray_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected
{
    using type = int[6];
    using traits = ttl::container_traits<type>;

    STATIC_ASSERT(traits::is_container);
    STATIC_ASSERT(traits::is_writable);
    STATIC_ASSERT(!traits::is_byte_address);
    STATIC_ASSERT(!traits::is_structured);
    STATIC_ASSERT(!traits::is_resource);
    STATIC_ASSERT((ttl::is_same<traits::element_type, int>::value));
    STATIC_ASSERT((ttl::is_same<traits::type, type>::value));
}

namespace GIVEN_NonArray_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected
{
    using type = int;
    using traits = ttl::container_traits<type>;

    STATIC_ASSERT(!traits::is_container);
    STATIC_ASSERT(!traits::is_writable);
    STATIC_ASSERT(!traits::is_byte_address);
    STATIC_ASSERT(!traits::is_structured);
    STATIC_ASSERT(!traits::is_resource);
    STATIC_ASSERT((ttl::is_same<traits::element_type, void>::value));
    STATIC_ASSERT((ttl::is_same<traits::type, void>::value));
}

namespace GIVEN_VectorType_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected
{
    using type = int2;
    using traits = ttl::container_traits<type>;

    STATIC_ASSERT(!traits::is_container);
    STATIC_ASSERT(!traits::is_writable);
    STATIC_ASSERT(!traits::is_byte_address);
    STATIC_ASSERT(!traits::is_structured);
    STATIC_ASSERT(!traits::is_resource);
    STATIC_ASSERT((ttl::is_same<traits::element_type, void>::value));
    STATIC_ASSERT((ttl::is_same<traits::type, void>::value));
}

namespace GIVEN_Buffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected
{
    using type = Buffer<int>;
    using traits = ttl::container_traits<type>;

    STATIC_ASSERT(traits::is_container);
    STATIC_ASSERT(!traits::is_writable);
    STATIC_ASSERT(!traits::is_byte_address);
    STATIC_ASSERT(!traits::is_structured);
    STATIC_ASSERT(traits::is_resource);
    STATIC_ASSERT((ttl::is_same<traits::element_type, int>::value));
    STATIC_ASSERT((ttl::is_same<traits::type, type>::value));
}

namespace GIVEN_RWBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected
{
    using type = RWBuffer<int>;
    using traits = ttl::container_traits<type>;
    STATIC_ASSERT(traits::is_container);
    STATIC_ASSERT(traits::is_writable);
    STATIC_ASSERT(!traits::is_byte_address);
    STATIC_ASSERT(!traits::is_structured);
    STATIC_ASSERT(traits::is_resource);
    STATIC_ASSERT((ttl::is_same<traits::element_type, int>::value));
    STATIC_ASSERT((ttl::is_same<traits::type, type>::value));
}

struct MyStruct
{
    int a;
    float b;
};

namespace GIVEN_StructuredBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected
{
    using type = StructuredBuffer<MyStruct>;
    using traits = ttl::container_traits<type>;

    STATIC_ASSERT(traits::is_container);
    STATIC_ASSERT(!traits::is_writable);
    STATIC_ASSERT(!traits::is_byte_address);
    STATIC_ASSERT(traits::is_structured);
    STATIC_ASSERT(traits::is_resource);
    STATIC_ASSERT((ttl::is_same<traits::element_type, MyStruct>::value));
    STATIC_ASSERT((ttl::is_same<traits::type, type>::value));
}

namespace GIVEN_RWStructuredBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected
{
    using type = RWStructuredBuffer<MyStruct>;
    using traits = ttl::container_traits<type>;

    STATIC_ASSERT(traits::is_container);
    STATIC_ASSERT(traits::is_writable);
    STATIC_ASSERT(!traits::is_byte_address);
    STATIC_ASSERT(traits::is_structured);
    STATIC_ASSERT(traits::is_resource);
    STATIC_ASSERT((ttl::is_same<traits::element_type, MyStruct>::value));
    STATIC_ASSERT((ttl::is_same<traits::type, type>::value));
}

namespace GIVEN_ByteAddressBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected
{
    using type = ByteAddressBuffer;
    using traits = ttl::container_traits<type>;

    STATIC_ASSERT(traits::is_container);
    STATIC_ASSERT(!traits::is_writable);
    STATIC_ASSERT(traits::is_byte_address);
    STATIC_ASSERT(!traits::is_structured);
    STATIC_ASSERT(traits::is_resource);
    STATIC_ASSERT((ttl::is_same<traits::element_type, uint>::value));
    STATIC_ASSERT((ttl::is_same<traits::type, type>::value));
}

namespace GIVEN_RWByteAddressBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected
{
    using type = RWByteAddressBuffer;
    using traits = ttl::container_traits<type>;

    STATIC_ASSERT(traits::is_container);
    STATIC_ASSERT(traits::is_writable);
    STATIC_ASSERT(traits::is_byte_address);
    STATIC_ASSERT(!traits::is_structured);
    STATIC_ASSERT(traits::is_resource);
    STATIC_ASSERT((ttl::is_same<traits::element_type, uint>::value));
    STATIC_ASSERT((ttl::is_same<traits::type, type>::value));
}
