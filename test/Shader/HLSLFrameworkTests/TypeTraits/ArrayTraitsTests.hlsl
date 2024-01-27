#include "/Test/TTL/static_assert.hlsli"
#include "/Test/TTL/type_traits.hlsli"


void GIVEN_BuiltInArray_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    static const uint size = 6;
    using type = int[size];
    using traits = ttl::array_traits<type>;

    STATIC_ASSERT(traits::is_array);
    STATIC_ASSERT(traits::size == size);
    STATIC_ASSERT((ttl::is_same<traits::element_type, int>::value));
}


void GIVEN_NonArray_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    using type = int;
    using traits = ttl::array_traits<type>;

    STATIC_ASSERT(!traits::is_array);
    STATIC_ASSERT(traits::size == 0u);
    STATIC_ASSERT((ttl::is_same<traits::element_type, void>::value));
}


void GIVEN_VectorType_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    using type = int2;
    using traits = ttl::array_traits<type>;

    STATIC_ASSERT(!traits::is_array);
    STATIC_ASSERT(traits::size == 0u);
    STATIC_ASSERT((ttl::is_same<traits::element_type, void>::value));
}


void GIVEN_Buffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    using type = Buffer<int>;
    using traits = ttl::array_traits<type>;

    STATIC_ASSERT(!traits::is_array);
    STATIC_ASSERT(traits::size == 0u);
    STATIC_ASSERT((ttl::is_same<traits::element_type, void>::value));
}


void GIVEN_RWBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    using type = RWBuffer<int>;
    using traits = ttl::array_traits<type>;

    STATIC_ASSERT(!traits::is_array);
    STATIC_ASSERT(traits::size == 0u);
    STATIC_ASSERT((ttl::is_same<traits::element_type, void>::value));
}

struct MyStruct
{
    int a;
    float b;
};


void GIVEN_StructuredBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    using type = StructuredBuffer<MyStruct>;
    using traits = ttl::array_traits<type>;

    STATIC_ASSERT(!traits::is_array);
    STATIC_ASSERT(traits::size == 0u);
    STATIC_ASSERT((ttl::is_same<traits::element_type, void>::value));
}


void GIVEN_RWStructuredBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    using type = RWStructuredBuffer<MyStruct>;
    using traits = ttl::array_traits<type>;

    STATIC_ASSERT(!traits::is_array);
    STATIC_ASSERT(traits::size == 0u);
    STATIC_ASSERT((ttl::is_same<traits::element_type, void>::value));
}


void GIVEN_ByteAddressBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    using type = ByteAddressBuffer;
    using traits = ttl::array_traits<type>;

    STATIC_ASSERT(!traits::is_array);
    STATIC_ASSERT(traits::size == 0u);
    STATIC_ASSERT((ttl::is_same<traits::element_type, void>::value));
}


void GIVEN_RWByteAddressBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected()
{
    using type = RWByteAddressBuffer;
    using traits = ttl::array_traits<type>;

    STATIC_ASSERT(!traits::is_array);
    STATIC_ASSERT(traits::size == 0u);
    STATIC_ASSERT((ttl::is_same<traits::element_type, void>::value));
}

[numthreads(1,1,1)]
void AllCompile()
{
    
}
