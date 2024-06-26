#include "/Test/TTL/type_traits.hlsli"

namespace AlignOfTests
{
    struct UnknownAlignmentLessThan8
    {
        int A;
    };

    struct UnknownAlignmentGreaterThan8Multiple2
    {
        int16_t A;
        int16_t B;
        int16_t C;
        int16_t D;
        int16_t E;
    };

    struct UnknownAlignmentGreaterThan8Multiple4
    {
        int A;
        int B;
        int C;
    };

    struct UnknownAlignmentGreaterThan8Multiple8
    {
        int A;
        int B;
        int C;
        int D;
    };

    struct LargeWithMultipleAlignments
    {
        int64_t A;
        int32_t B;
        int32_t C;
        int32_t D;
        int16_t E;
    };
}

namespace AlignOfTests
{
    _Static_assert(2u == ttl::align_of<int16_t>::value);
    _Static_assert(4u == ttl::align_of<int32_t>::value);
    _Static_assert(8u == ttl::align_of<int64_t>::value);
    _Static_assert(4u == ttl::align_of<bool>::value);

    _Static_assert(2u == ttl::align_of<int16_t3>::value);
    _Static_assert(8u == ttl::align_of<int64_t4>::value);

    _Static_assert(2u == ttl::align_of<int16_t3x4>::value);
    _Static_assert(8u == ttl::align_of<int64_t4x2>::value);

    _Static_assert((uint)sizeof(UnknownAlignmentLessThan8) == ttl::align_of<UnknownAlignmentLessThan8>::value);

    _Static_assert(2u == ttl::align_of<UnknownAlignmentGreaterThan8Multiple2>::value);
    _Static_assert(4u == ttl::align_of<UnknownAlignmentGreaterThan8Multiple4>::value);
    _Static_assert(4u == ttl::align_of<UnknownAlignmentGreaterThan8Multiple8>::value);

    _Static_assert(8u == ttl::align_of<LargeWithMultipleAlignments>::value);

    _Static_assert(2u == ttl::align_of<int16_t[10]>::value);
    _Static_assert(4u == ttl::align_of<int32_t[10]>::value);
    _Static_assert(8u == ttl::align_of<int64_t[10]>::value);

    _Static_assert(2u == ttl::align_of_v<int16_t>);
    _Static_assert(4u == ttl::align_of_v<int32_t>);
    _Static_assert(8u == ttl::align_of_v<int64_t>);
    _Static_assert(4u == ttl::align_of_v<bool>);
}

namespace ArrayTraitsTests
{
    namespace GIVEN_BuiltInArray_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected
    {
        static const uint size = 6;
        using type = int[size];
        using traits = ttl::array_traits<type>;

        _Static_assert(traits::is_array);
        _Static_assert(traits::size == size);
        _Static_assert(ttl::is_same<traits::element_type, int>::value);
    }


    namespace GIVEN_NonArray_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected
    {
        using type = int;
        using traits = ttl::array_traits<type>;

        _Static_assert(!traits::is_array);
        _Static_assert(traits::size == 0u);
        _Static_assert(ttl::is_same<traits::element_type, void>::value);
    }


    namespace GIVEN_VectorType_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected
    {
        using type = int2;
        using traits = ttl::array_traits<type>;

        _Static_assert(!traits::is_array);
        _Static_assert(traits::size == 0u);
        _Static_assert(ttl::is_same<traits::element_type, void>::value);
    }


    namespace GIVEN_Buffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected
    {
        using type = Buffer<int>;
        using traits = ttl::array_traits<type>;

        _Static_assert(!traits::is_array);
        _Static_assert(traits::size == 0u);
        _Static_assert(ttl::is_same<traits::element_type, void>::value);
    }


    namespace GIVEN_RWBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected
    {
        using type = RWBuffer<int>;
        using traits = ttl::array_traits<type>;

        _Static_assert(!traits::is_array);
        _Static_assert(traits::size == 0u);
        _Static_assert(ttl::is_same<traits::element_type, void>::value);
    }

    struct MyStruct
    {
        int a;
        float b;
    };


    namespace GIVEN_StructuredBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected
    {
        using type = StructuredBuffer<MyStruct>;
        using traits = ttl::array_traits<type>;

        _Static_assert(!traits::is_array);
        _Static_assert(traits::size == 0u);
        _Static_assert(ttl::is_same<traits::element_type, void>::value);
    }


    namespace GIVEN_RWStructuredBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected
    {
        using type = RWStructuredBuffer<MyStruct>;
        using traits = ttl::array_traits<type>;

        _Static_assert(!traits::is_array);
        _Static_assert(traits::size == 0u);
        _Static_assert(ttl::is_same<traits::element_type, void>::value);
    }


    namespace GIVEN_ByteAddressBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected
    {
        using type = ByteAddressBuffer;
        using traits = ttl::array_traits<type>;

        _Static_assert(!traits::is_array);
        _Static_assert(traits::size == 0u);
        _Static_assert(ttl::is_same<traits::element_type, void>::value);
    }


    namespace GIVEN_RWByteAddressBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected
    {
        using type = RWByteAddressBuffer;
        using traits = ttl::array_traits<type>;

        _Static_assert(!traits::is_array);
        _Static_assert(traits::size == 0u);
        _Static_assert(ttl::is_same<traits::element_type, void>::value);
    }
}

namespace ConditionalTests
{
    struct A{};
    struct B{};

    using TrueAB = ttl::conditional<true, A, B>::type;
    using FalseAB = ttl::conditional<false, A, B>::type;
    using TrueBA = ttl::conditional<true, B, A>::type;
    using FalseBA = ttl::conditional<false, B, A>::type;

    _Static_assert(ttl::is_same_v<TrueAB, A>);
    _Static_assert(ttl::is_same_v<FalseAB, B>);
    _Static_assert(ttl::is_same_v<TrueBA, B>);
    _Static_assert(ttl::is_same_v<FalseBA, A>);

    using TrueABv = ttl::conditional_t<true, A, B>;
    using FalseABv = ttl::conditional_t<false, A, B>;
    using TrueBAv = ttl::conditional_t<true, B, A>;
    using FalseBAv = ttl::conditional_t<false, B, A>;

    _Static_assert(ttl::is_same_v<TrueABv, A>);
    _Static_assert(ttl::is_same_v<FalseABv, B>);
    _Static_assert(ttl::is_same_v<TrueBAv, B>);
    _Static_assert(ttl::is_same_v<FalseBAv, A>);
}

namespace ContainerTraitsTests
{
    namespace GIVEN_BuiltInArray_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected
    {
        using type = int[6];
        using traits = ttl::container_traits<type>;

        _Static_assert(traits::is_container);
        _Static_assert(traits::is_writable);
        _Static_assert(!traits::is_byte_address);
        _Static_assert(!traits::is_structured);
        _Static_assert(!traits::is_resource);
        _Static_assert(ttl::is_same<traits::element_type, int>::value);
        _Static_assert(ttl::is_same<traits::type, type>::value);
    }

    namespace GIVEN_NonArray_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected
    {
        using type = int;
        using traits = ttl::container_traits<type>;

        _Static_assert(!traits::is_container);
        _Static_assert(!traits::is_writable);
        _Static_assert(!traits::is_byte_address);
        _Static_assert(!traits::is_structured);
        _Static_assert(!traits::is_resource);
        _Static_assert(ttl::is_same<traits::element_type, void>::value);
        _Static_assert(ttl::is_same<traits::type, void>::value);
    }

    namespace GIVEN_VectorType_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected
    {
        using type = int2;
        using traits = ttl::container_traits<type>;

        _Static_assert(!traits::is_container);
        _Static_assert(!traits::is_writable);
        _Static_assert(!traits::is_byte_address);
        _Static_assert(!traits::is_structured);
        _Static_assert(!traits::is_resource);
        _Static_assert(ttl::is_same<traits::element_type, void>::value);
        _Static_assert(ttl::is_same<traits::type, void>::value);
    }

    namespace GIVEN_Buffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected
    {
        using type = Buffer<int>;
        using traits = ttl::container_traits<type>;

        _Static_assert(traits::is_container);
        _Static_assert(!traits::is_writable);
        _Static_assert(!traits::is_byte_address);
        _Static_assert(!traits::is_structured);
        _Static_assert(traits::is_resource);
        _Static_assert(ttl::is_same<traits::element_type, int>::value);
        _Static_assert(ttl::is_same<traits::type, type>::value);
    }

    namespace GIVEN_RWBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected
    {
        using type = RWBuffer<int>;
        using traits = ttl::container_traits<type>;
        _Static_assert(traits::is_container);
        _Static_assert(traits::is_writable);
        _Static_assert(!traits::is_byte_address);
        _Static_assert(!traits::is_structured);
        _Static_assert(traits::is_resource);
        _Static_assert(ttl::is_same<traits::element_type, int>::value);
        _Static_assert(ttl::is_same<traits::type, type>::value);
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

        _Static_assert(traits::is_container);
        _Static_assert(!traits::is_writable);
        _Static_assert(!traits::is_byte_address);
        _Static_assert(traits::is_structured);
        _Static_assert(traits::is_resource);
        _Static_assert(ttl::is_same<traits::element_type, MyStruct>::value);
        _Static_assert(ttl::is_same<traits::type, type>::value);
    }

    namespace GIVEN_RWStructuredBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected
    {
        using type = RWStructuredBuffer<MyStruct>;
        using traits = ttl::container_traits<type>;

        _Static_assert(traits::is_container);
        _Static_assert(traits::is_writable);
        _Static_assert(!traits::is_byte_address);
        _Static_assert(traits::is_structured);
        _Static_assert(traits::is_resource);
        _Static_assert(ttl::is_same<traits::element_type, MyStruct>::value);
        _Static_assert(ttl::is_same<traits::type, type>::value);
    }

    namespace GIVEN_ByteAddressBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected
    {
        using type = ByteAddressBuffer;
        using traits = ttl::container_traits<type>;

        _Static_assert(traits::is_container);
        _Static_assert(!traits::is_writable);
        _Static_assert(traits::is_byte_address);
        _Static_assert(!traits::is_structured);
        _Static_assert(traits::is_resource);
        _Static_assert(ttl::is_same<traits::element_type, uint>::value);
        _Static_assert(ttl::is_same<traits::type, type>::value);
    }

    namespace GIVEN_RWByteAddressBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected
    {
        using type = RWByteAddressBuffer;
        using traits = ttl::container_traits<type>;

        _Static_assert(traits::is_container);
        _Static_assert(traits::is_writable);
        _Static_assert(traits::is_byte_address);
        _Static_assert(!traits::is_structured);
        _Static_assert(traits::is_resource);
        _Static_assert(ttl::is_same<traits::element_type, uint>::value);
        _Static_assert(ttl::is_same<traits::type, type>::value);
    }
}

namespace DeclvalTests
{
    struct A
    {
    };

    struct B
    {
    };

    _Static_assert(ttl::is_same<A, __decltype(ttl::declval<A>())>::value);
    _Static_assert(!ttl::is_same<B, __decltype(ttl::declval<A>())>::value);

    _Static_assert(!ttl::is_same<A, __decltype(ttl::declval<B>())>::value);
    _Static_assert(ttl::is_same<B, __decltype(ttl::declval<B>())>::value);
}

namespace FundamentalTypeTraitsTests
{
    struct MyStruct
    {
        int a;
    };

    void ScalarTests()
    {
        _Static_assert(ttl::fundamental_type_traits<bool>::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<int16_t>::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<int32_t>::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<int64_t>::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<uint16_t>::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<uint32_t>::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<uint64_t>::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<float16_t>::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<float32_t>::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<float64_t>::is_fundamental);
        _Static_assert(!ttl::fundamental_type_traits<MyStruct>::is_fundamental);

        _Static_assert(1u == ttl::fundamental_type_traits<bool>::dim0);
        _Static_assert(1u == ttl::fundamental_type_traits<int16_t>::dim0);
        _Static_assert(1u == ttl::fundamental_type_traits<int32_t>::dim0);
        _Static_assert(1u == ttl::fundamental_type_traits<int64_t>::dim0);
        _Static_assert(1u == ttl::fundamental_type_traits<uint16_t>::dim0);
        _Static_assert(1u == ttl::fundamental_type_traits<uint32_t>::dim0);
        _Static_assert(1u == ttl::fundamental_type_traits<uint64_t>::dim0);
        _Static_assert(1u == ttl::fundamental_type_traits<float16_t>::dim0);
        _Static_assert(1u == ttl::fundamental_type_traits<float32_t>::dim0);
        _Static_assert(1u == ttl::fundamental_type_traits<float64_t>::dim0);
        _Static_assert(0u == ttl::fundamental_type_traits<MyStruct>::dim0);

        _Static_assert(1u == ttl::fundamental_type_traits<bool>::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<int16_t>::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<int32_t>::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<int64_t>::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<uint16_t>::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<uint32_t>::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<uint64_t>::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<float16_t>::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<float32_t>::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<float64_t>::dim1);
        _Static_assert(0u == ttl::fundamental_type_traits<MyStruct>::dim1);

        _Static_assert(ttl::is_same<bool, ttl::fundamental_type_traits<bool>::base_type>::value);
        _Static_assert(ttl::is_same<int16_t, ttl::fundamental_type_traits<int16_t>::base_type>::value);
        _Static_assert(ttl::is_same<int32_t, ttl::fundamental_type_traits<int32_t>::base_type>::value);
        _Static_assert(ttl::is_same<int64_t, ttl::fundamental_type_traits<int64_t>::base_type>::value);
        _Static_assert(ttl::is_same<uint16_t, ttl::fundamental_type_traits<uint16_t>::base_type>::value);
        _Static_assert(ttl::is_same<uint32_t, ttl::fundamental_type_traits<uint32_t>::base_type>::value);
        _Static_assert(ttl::is_same<uint64_t, ttl::fundamental_type_traits<uint64_t>::base_type>::value);
        _Static_assert(ttl::is_same<float16_t, ttl::fundamental_type_traits<float16_t>::base_type>::value);
        _Static_assert(ttl::is_same<float32_t, ttl::fundamental_type_traits<float32_t>::base_type>::value);
        _Static_assert(ttl::is_same<float64_t, ttl::fundamental_type_traits<float64_t>::base_type>::value);
        _Static_assert(ttl::is_same<void, ttl::fundamental_type_traits<MyStruct>::base_type>::value);
    }

    template<uint InDim>
    void VectorTests()
    {
        _Static_assert(ttl::fundamental_type_traits<vector<bool, InDim> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<vector<int16_t, InDim> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<vector<int32_t, InDim> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<vector<int64_t, InDim> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<vector<uint16_t, InDim> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<vector<uint32_t, InDim> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<vector<uint64_t, InDim> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<vector<float16_t, InDim> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<vector<float32_t, InDim> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<vector<float64_t, InDim> >::is_fundamental);

        _Static_assert(InDim == ttl::fundamental_type_traits<vector<bool, InDim> >::dim0);
        _Static_assert(InDim == ttl::fundamental_type_traits<vector<int16_t, InDim> >::dim0);
        _Static_assert(InDim == ttl::fundamental_type_traits<vector<int32_t, InDim> >::dim0);
        _Static_assert(InDim == ttl::fundamental_type_traits<vector<int64_t, InDim> >::dim0);
        _Static_assert(InDim == ttl::fundamental_type_traits<vector<uint16_t, InDim> >::dim0);
        _Static_assert(InDim == ttl::fundamental_type_traits<vector<uint32_t, InDim> >::dim0);
        _Static_assert(InDim == ttl::fundamental_type_traits<vector<uint64_t, InDim> >::dim0);
        _Static_assert(InDim == ttl::fundamental_type_traits<vector<float16_t, InDim> >::dim0);
        _Static_assert(InDim == ttl::fundamental_type_traits<vector<float32_t, InDim> >::dim0);
        _Static_assert(InDim == ttl::fundamental_type_traits<vector<float64_t, InDim> >::dim0);

        _Static_assert(1u == ttl::fundamental_type_traits<vector<bool, InDim> >::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<vector<int16_t, InDim> >::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<vector<int32_t, InDim> >::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<vector<int64_t, InDim> >::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<vector<uint16_t, InDim> >::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<vector<uint32_t, InDim> >::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<vector<uint64_t, InDim> >::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<vector<float16_t, InDim> >::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<vector<float32_t, InDim> >::dim1);
        _Static_assert(1u == ttl::fundamental_type_traits<vector<float64_t, InDim> >::dim1);

        _Static_assert(ttl::is_same<bool, typename ttl::fundamental_type_traits<vector<bool, InDim> >::base_type>::value);
        _Static_assert(ttl::is_same<int16_t, typename ttl::fundamental_type_traits<vector<int16_t, InDim> >::base_type>::value);
        _Static_assert(ttl::is_same<int32_t, typename ttl::fundamental_type_traits<vector<int32_t, InDim> >::base_type>::value);
        _Static_assert(ttl::is_same<int64_t, typename ttl::fundamental_type_traits<vector<int64_t, InDim> >::base_type>::value);
        _Static_assert(ttl::is_same<uint16_t, typename ttl::fundamental_type_traits<vector<uint16_t, InDim> >::base_type>::value);
        _Static_assert(ttl::is_same<uint32_t, typename ttl::fundamental_type_traits<vector<uint32_t, InDim> >::base_type>::value);
        _Static_assert(ttl::is_same<uint64_t, typename ttl::fundamental_type_traits<vector<uint64_t, InDim> >::base_type>::value);
        _Static_assert(ttl::is_same<float16_t, typename ttl::fundamental_type_traits<vector<float16_t, InDim> >::base_type>::value);
        _Static_assert(ttl::is_same<float32_t, typename ttl::fundamental_type_traits<vector<float32_t, InDim> >::base_type>::value);
        _Static_assert(ttl::is_same<float64_t, typename ttl::fundamental_type_traits<vector<float64_t, InDim> >::base_type>::value);
    }

    template<uint InDim0, uint InDim1>
    void MatrixTests()
    {
        _Static_assert(ttl::fundamental_type_traits<matrix<bool, InDim0, InDim1> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<matrix<int16_t, InDim0, InDim1> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<matrix<int32_t, InDim0, InDim1> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<matrix<int64_t, InDim0, InDim1> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<matrix<uint16_t, InDim0, InDim1> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<matrix<uint32_t, InDim0, InDim1> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<matrix<uint64_t, InDim0, InDim1> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<matrix<float16_t, InDim0, InDim1> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<matrix<float32_t, InDim0, InDim1> >::is_fundamental);
        _Static_assert(ttl::fundamental_type_traits<matrix<float64_t, InDim0, InDim1> >::is_fundamental);

        _Static_assert(InDim0 == ttl::fundamental_type_traits<matrix<bool, InDim0, InDim1> >::dim0);
        _Static_assert(InDim0 == ttl::fundamental_type_traits<matrix<int16_t, InDim0, InDim1> >::dim0);
        _Static_assert(InDim0 == ttl::fundamental_type_traits<matrix<int32_t, InDim0, InDim1> >::dim0);
        _Static_assert(InDim0 == ttl::fundamental_type_traits<matrix<int64_t, InDim0, InDim1> >::dim0);
        _Static_assert(InDim0 == ttl::fundamental_type_traits<matrix<uint16_t, InDim0, InDim1> >::dim0);
        _Static_assert(InDim0 == ttl::fundamental_type_traits<matrix<uint32_t, InDim0, InDim1> >::dim0);
        _Static_assert(InDim0 == ttl::fundamental_type_traits<matrix<uint64_t, InDim0, InDim1> >::dim0);
        _Static_assert(InDim0 == ttl::fundamental_type_traits<matrix<float16_t, InDim0, InDim1> >::dim0);
        _Static_assert(InDim0 == ttl::fundamental_type_traits<matrix<float32_t, InDim0, InDim1> >::dim0);
        _Static_assert(InDim0 == ttl::fundamental_type_traits<matrix<float64_t, InDim0, InDim1> >::dim0);

        _Static_assert(InDim1 == ttl::fundamental_type_traits<matrix<bool, InDim0, InDim1> >::dim1);
        _Static_assert(InDim1 == ttl::fundamental_type_traits<matrix<int16_t, InDim0, InDim1> >::dim1);
        _Static_assert(InDim1 == ttl::fundamental_type_traits<matrix<int32_t, InDim0, InDim1> >::dim1);
        _Static_assert(InDim1 == ttl::fundamental_type_traits<matrix<int64_t, InDim0, InDim1> >::dim1);
        _Static_assert(InDim1 == ttl::fundamental_type_traits<matrix<uint16_t, InDim0, InDim1> >::dim1);
        _Static_assert(InDim1 == ttl::fundamental_type_traits<matrix<uint32_t, InDim0, InDim1> >::dim1);
        _Static_assert(InDim1 == ttl::fundamental_type_traits<matrix<uint64_t, InDim0, InDim1> >::dim1);
        _Static_assert(InDim1 == ttl::fundamental_type_traits<matrix<float16_t, InDim0, InDim1> >::dim1);
        _Static_assert(InDim1 == ttl::fundamental_type_traits<matrix<float32_t, InDim0, InDim1> >::dim1);
        _Static_assert(InDim1 == ttl::fundamental_type_traits<matrix<float64_t, InDim0, InDim1> >::dim1);

        _Static_assert(ttl::is_same<bool, typename ttl::fundamental_type_traits<matrix<bool, InDim0, InDim1> >::base_type>::value);
        _Static_assert(ttl::is_same<int16_t, typename ttl::fundamental_type_traits<matrix<int16_t, InDim0, InDim1> >::base_type>::value);
        _Static_assert(ttl::is_same<int32_t, typename ttl::fundamental_type_traits<matrix<int32_t, InDim0, InDim1> >::base_type>::value);
        _Static_assert(ttl::is_same<int64_t, typename ttl::fundamental_type_traits<matrix<int64_t, InDim0, InDim1> >::base_type>::value);
        _Static_assert(ttl::is_same<uint16_t, typename ttl::fundamental_type_traits<matrix<uint16_t, InDim0, InDim1> >::base_type>::value);
        _Static_assert(ttl::is_same<uint32_t, typename ttl::fundamental_type_traits<matrix<uint32_t, InDim0, InDim1> >::base_type>::value);
        _Static_assert(ttl::is_same<uint64_t, typename ttl::fundamental_type_traits<matrix<uint64_t, InDim0, InDim1> >::base_type>::value);
        _Static_assert(ttl::is_same<float16_t, typename ttl::fundamental_type_traits<matrix<float16_t, InDim0, InDim1> >::base_type>::value);
        _Static_assert(ttl::is_same<float32_t, typename ttl::fundamental_type_traits<matrix<float32_t, InDim0, InDim1> >::base_type>::value);
        _Static_assert(ttl::is_same<float64_t, typename ttl::fundamental_type_traits<matrix<float64_t, InDim0, InDim1> >::base_type>::value);
    }

    void RunScalarTests()
    {
        ScalarTests();
    }

    void RunVectorTests()
    {
        VectorTests<1>();
        VectorTests<2>();
        VectorTests<3>();
        VectorTests<4>();
    }

    void RunMatrix1Tests()
    {
        MatrixTests<1, 1>();
        MatrixTests<2, 1>();
        MatrixTests<3, 1>();
        MatrixTests<4, 1>();
    }

    void RunMatrix2Tests()
    {
        MatrixTests<1, 2>();
        MatrixTests<2, 2>();
        MatrixTests<3, 2>();
        MatrixTests<4, 2>();
    }

    void RunMatrix3Tests()
    {
        MatrixTests<1, 3>();
        MatrixTests<2, 3>();
        MatrixTests<3, 3>();
        MatrixTests<4, 3>();
    }

    void RunMatrix4Tests()
    {
        MatrixTests<1, 4>();
        MatrixTests<2, 4>();
        MatrixTests<3, 4>();
        MatrixTests<4, 4>();
    }
}

namespace IsOrHasEnumTests
{
    enum A
    {
        One
    };

    enum class B
    {
        One
    };

    struct C
    {
        int D;
    };
                                                                                           
    struct D
    {
        B data;
    };
                                                                                             
    _Static_assert(ttl::is_or_has_enum<A>::value);
    _Static_assert(ttl::is_or_has_enum<B>::value);
    _Static_assert(!ttl::is_or_has_enum<C>::value);
    _Static_assert(!ttl::is_or_has_enum<uint>::value);
    _Static_assert(ttl::is_or_has_enum<D>::value);

    _Static_assert(ttl::is_or_has_enum_v<A>);
    _Static_assert(ttl::is_or_has_enum_v<B>);
    _Static_assert(!ttl::is_or_has_enum_v<C>);
    _Static_assert(!ttl::is_or_has_enum_v<uint>);
    _Static_assert(ttl::is_or_has_enum_v<D>);
}

namespace IsFunctionTests
{
    struct ParamType{};
    struct NotParamType{};

    struct Functor
    {
        void operator()(){}
    };

    void ZeroParamFunction(){}
    void OneParamFunction(ParamType){}
    void TwoParamFunction(ParamType, ParamType){}
    void ThreeParamFunction(ParamType, ParamType, ParamType){}
    void FourParamFunction(ParamType, ParamType, ParamType, ParamType){}
    void FiveParamFunction(ParamType, ParamType, ParamType, ParamType, ParamType){}

    _Static_assert(ttl::is_function<__decltype(ZeroParamFunction)>::value);
    _Static_assert(ttl::is_function<__decltype(OneParamFunction)>::value);
    _Static_assert(ttl::is_function<__decltype(TwoParamFunction)>::value);
    _Static_assert(ttl::is_function<__decltype(ThreeParamFunction)>::value);
    _Static_assert(ttl::is_function<__decltype(FourParamFunction)>::value);
    _Static_assert(ttl::is_function<__decltype(FiveParamFunction)>::value);

    _Static_assert(!ttl::is_function<Functor>::value);
    _Static_assert(!ttl::is_function<ParamType>::value);

    _Static_assert(ttl::is_function_v<__decltype(ZeroParamFunction)>);
    _Static_assert(ttl::is_function_v<__decltype(OneParamFunction)>);
    _Static_assert(ttl::is_function_v<__decltype(TwoParamFunction)>);
    _Static_assert(ttl::is_function_v<__decltype(ThreeParamFunction)>);
    _Static_assert(ttl::is_function_v<__decltype(FourParamFunction)>);
    _Static_assert(ttl::is_function_v<__decltype(FiveParamFunction)>);

    _Static_assert(!ttl::is_function_v<Functor>);
    _Static_assert(!ttl::is_function_v<ParamType>);
}

namespace IsInvocableFunctionTests
{
    struct ParamType{};
    struct NotParamType{};

    void ZeroParamFunction(){}
    void OneParamFunction(ParamType){}
    void TwoParamFunction(ParamType, ParamType){}
    void ThreeParamFunction(ParamType, ParamType, ParamType){}
    void FourParamFunction(ParamType, ParamType, ParamType, ParamType){}
    void FiveParamFunction(ParamType, ParamType, ParamType, ParamType, ParamType){}

    _Static_assert(ttl::is_invocable_function<__decltype(ZeroParamFunction)>::value);
    _Static_assert(ttl::is_invocable_function<__decltype(OneParamFunction), ParamType>::value);
    _Static_assert(ttl::is_invocable_function<__decltype(TwoParamFunction), ParamType, ParamType>::value);
    _Static_assert(ttl::is_invocable_function<__decltype(ThreeParamFunction), ParamType, ParamType, ParamType>::value);
    _Static_assert(ttl::is_invocable_function<__decltype(FourParamFunction), ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(ttl::is_invocable_function<__decltype(FiveParamFunction), ParamType, ParamType, ParamType, ParamType, ParamType>::value);

    _Static_assert(!ttl::is_invocable_function<__decltype(OneParamFunction)>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(TwoParamFunction)>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(ThreeParamFunction)>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(FourParamFunction)>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(FiveParamFunction)>::value);

    _Static_assert(!ttl::is_invocable_function<__decltype(ZeroParamFunction), NotParamType>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(OneParamFunction), NotParamType>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(TwoParamFunction), NotParamType, ParamType>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(ThreeParamFunction), NotParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(FourParamFunction), NotParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(FiveParamFunction), NotParamType, ParamType, ParamType, ParamType, ParamType>::value);

    _Static_assert(!ttl::is_invocable_function<__decltype(TwoParamFunction), ParamType, NotParamType>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(ThreeParamFunction), ParamType, NotParamType, ParamType>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(FourParamFunction), ParamType, NotParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(FiveParamFunction), ParamType, NotParamType, ParamType, ParamType, ParamType>::value);

    _Static_assert(!ttl::is_invocable_function<__decltype(ThreeParamFunction), ParamType, ParamType, NotParamType>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(FourParamFunction), ParamType, ParamType, NotParamType, ParamType>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(FiveParamFunction), ParamType, ParamType, NotParamType, ParamType, ParamType>::value);

    _Static_assert(!ttl::is_invocable_function<__decltype(FourParamFunction), ParamType, ParamType, ParamType, NotParamType>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(FiveParamFunction), ParamType, ParamType, ParamType, NotParamType, ParamType>::value);

    _Static_assert(!ttl::is_invocable_function<__decltype(FiveParamFunction), ParamType, ParamType, ParamType, ParamType, NotParamType>::value);

    _Static_assert(!ttl::is_invocable_function<__decltype(OneParamFunction), NotParamType>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(TwoParamFunction), NotParamType, NotParamType>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(ThreeParamFunction), NotParamType, NotParamType, NotParamType>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(FourParamFunction), NotParamType, NotParamType, NotParamType, NotParamType>::value);
    _Static_assert(!ttl::is_invocable_function<__decltype(FiveParamFunction), NotParamType, NotParamType, NotParamType, NotParamType, NotParamType>::value);

    _Static_assert(ttl::is_invocable_function_v<__decltype(ZeroParamFunction)>);
    _Static_assert(ttl::is_invocable_function_v<__decltype(OneParamFunction), ParamType>);
    _Static_assert(ttl::is_invocable_function_v<__decltype(TwoParamFunction), ParamType, ParamType>);
    _Static_assert(ttl::is_invocable_function_v<__decltype(ThreeParamFunction), ParamType, ParamType, ParamType>);
    _Static_assert(ttl::is_invocable_function_v<__decltype(FourParamFunction), ParamType, ParamType, ParamType, ParamType>);
    _Static_assert(ttl::is_invocable_function_v<__decltype(FiveParamFunction), ParamType, ParamType, ParamType, ParamType, ParamType>);

    _Static_assert(!ttl::is_invocable_function_v<__decltype(OneParamFunction)>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(TwoParamFunction)>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(ThreeParamFunction)>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(FourParamFunction)>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(FiveParamFunction)>);

    _Static_assert(!ttl::is_invocable_function_v<__decltype(ZeroParamFunction), NotParamType>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(OneParamFunction), NotParamType>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(TwoParamFunction), NotParamType, ParamType>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(ThreeParamFunction), NotParamType, ParamType, ParamType>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(FourParamFunction), NotParamType, ParamType, ParamType, ParamType>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(FiveParamFunction), NotParamType, ParamType, ParamType, ParamType, ParamType>);

    _Static_assert(!ttl::is_invocable_function_v<__decltype(TwoParamFunction), ParamType, NotParamType>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(ThreeParamFunction), ParamType, NotParamType, ParamType>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(FourParamFunction), ParamType, NotParamType, ParamType, ParamType>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(FiveParamFunction), ParamType, NotParamType, ParamType, ParamType, ParamType>);

    _Static_assert(!ttl::is_invocable_function_v<__decltype(ThreeParamFunction), ParamType, ParamType, NotParamType>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(FourParamFunction), ParamType, ParamType, NotParamType, ParamType>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(FiveParamFunction), ParamType, ParamType, NotParamType, ParamType, ParamType>);

    _Static_assert(!ttl::is_invocable_function_v<__decltype(FourParamFunction), ParamType, ParamType, ParamType, NotParamType>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(FiveParamFunction), ParamType, ParamType, ParamType, NotParamType, ParamType>);

    _Static_assert(!ttl::is_invocable_function_v<__decltype(FiveParamFunction), ParamType, ParamType, ParamType, ParamType, NotParamType>);

    _Static_assert(!ttl::is_invocable_function_v<__decltype(OneParamFunction), NotParamType>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(TwoParamFunction), NotParamType, NotParamType>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(ThreeParamFunction), NotParamType, NotParamType, NotParamType>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(FourParamFunction), NotParamType, NotParamType, NotParamType, NotParamType>);
    _Static_assert(!ttl::is_invocable_function_v<__decltype(FiveParamFunction), NotParamType, NotParamType, NotParamType, NotParamType, NotParamType>);
}

namespace IsSameTests
{
    struct A{};
    struct B{};

    using Alias = A;

    _Static_assert(ttl::is_same<A, A>::value, "Returns true because both arguments are A");
    _Static_assert(ttl::is_same<A, Alias>::value, "Returns true because we are comparing an alias to A and A");
    _Static_assert(!ttl::is_same<A, B>::value, "Returns false because A and B are different types");

    _Static_assert(ttl::is_same_v<A, A>, "Returns true because both arguments are A");
    _Static_assert(ttl::is_same_v<A, Alias>, "Returns true because we are comparing an alias to A and A");
    _Static_assert(!ttl::is_same_v<A, B>, "Returns false because A and B are different types");
}

namespace SizeOfTests
{
    enum A
    {
        One
    };

    enum class B
    {
        One
    };

    struct C
    {
        int D;
    };

    struct D
    {
        A data;
    };

    struct E
    {
        A data1;
        B data2;
        float4 data3;
    };

    _Static_assert(4u == ttl::size_of<A>::value);
    _Static_assert(4u == ttl::size_of<B>::value);
    _Static_assert(4u == ttl::size_of<C>::value);
    _Static_assert(4u == ttl::size_of<uint>::value);
    _Static_assert(8u == ttl::size_of<uint64_t>::value);
    _Static_assert(2u == ttl::size_of<uint16_t>::value);
    _Static_assert(24u == ttl::size_of<uint64_t3>::value);
    _Static_assert(8u == ttl::size_of<uint16_t4>::value);

    _Static_assert(4u == ttl::size_of<D>::value);
    _Static_assert(24u == ttl::size_of<E>::value);

    _Static_assert(4u == ttl::size_of_v<A>);
    _Static_assert(4u == ttl::size_of_v<B>);
    _Static_assert(4u == ttl::size_of_v<C>);
    _Static_assert(4u == ttl::size_of_v<uint>);
    _Static_assert(8u == ttl::size_of_v<uint64_t>);
    _Static_assert(2u == ttl::size_of_v<uint16_t>);
    _Static_assert(24u == ttl::size_of_v<uint64_t3>);
    _Static_assert(8u == ttl::size_of_v<uint16_t4>);

    _Static_assert(4u == ttl::size_of_v<D>);
    _Static_assert(24u == ttl::size_of_v<E>);
}

namespace EnableIfTests
{
    void Foo()
    {
    }

    template<typename T, typename = void>
    struct A
    {
        static const uint value = 0;
    };

    template<typename T>
    struct A<T, typename ttl::enable_if<ttl::is_array<T>::value>::type>
    {
        static const uint value = 1;
    };

    template<typename T>
    struct A<T, ttl::enable_if_t<ttl::is_function<T>::value> >
    {
        static const uint value = 2;
    };

    _Static_assert(A<int>::value == 0, "value is 0 because int is neither an array or a function so it takes the primary template");
    _Static_assert(A<float[42]>::value == 1, "value is 1 because float[42] is an array so it takes the first specialization");
    _Static_assert(A<__decltype(Foo)>::value == 2, "value is 2 because Foo is a function so it takes the second specialization");
}

namespace VoidTTests
{
    struct A
    {
        using type = void;
    };

    struct B
    {

    };

    _Static_assert(ttl::is_same<void, ttl::void_t<> >::value);
    _Static_assert(ttl::is_same<void, ttl::void_t<A> >::value);
    _Static_assert(ttl::is_same<void, ttl::void_t<A, A> >::value);
    _Static_assert(ttl::is_same<void, ttl::void_t<A, A, A> >::value);
    _Static_assert(ttl::is_same<void, ttl::void_t<A, A, A, A> >::value);
    _Static_assert(ttl::is_same<void, ttl::void_t<A, A, A, A, A> >::value);
    _Static_assert(ttl::is_same<void, ttl::void_t<A, A, A, A, A, A> >::value);
    _Static_assert(ttl::is_same<void, ttl::void_t<A, A, A, A, A, A, A> >::value);
    _Static_assert(ttl::is_same<void, ttl::void_t<A, A, A, A, A, A, A, A> >::value);
    _Static_assert(ttl::is_same<void, ttl::void_t<A, A, A, A, A, A, A, A, A> >::value);
    _Static_assert(ttl::is_same<void, ttl::void_t<A, A, A, A, A, A, A, A, A, A> >::value);

    template<
        typename T0 = A, typename T1 = A , typename T2 = A, typename T3 = A, typename T4 = A,
        typename T5 = A, typename T6 = A , typename T7 = A, typename T8 = A, typename T9 = A, typename = void
        >
    struct VoidTDetector
    {
        static const bool value = false;
    };


    template<
        typename T0, typename T1, typename T2, typename T3, typename T4,
        typename T5, typename T6, typename T7, typename T8, typename T9
        >
    struct VoidTDetector<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
        ttl::void_t<typename T0::type, typename T1::type, typename T2::type, typename T3::type, typename T4::type, typename T5::type, typename T6::type, typename T7::type, typename T8::type, typename T9::type> >
    {
        static const bool value = true;
    };

    _Static_assert(VoidTDetector<A>::value);
    _Static_assert(!VoidTDetector<B>::value);
    _Static_assert(!VoidTDetector<A, B>::value);
    _Static_assert(!VoidTDetector<B, A>::value);
    _Static_assert(!VoidTDetector<A, A, B>::value);
    _Static_assert(!VoidTDetector<A, A, A, B>::value);
    _Static_assert(!VoidTDetector<A, A, A, A, B>::value);
    _Static_assert(!VoidTDetector<A, A, A, A, A, B>::value);
    _Static_assert(!VoidTDetector<A, A, A, A, A, A, B>::value);
    _Static_assert(!VoidTDetector<A, A, A, A, A, A, A, B>::value);
    _Static_assert(!VoidTDetector<A, A, A, A, A, A, A, A, B>::value);
    _Static_assert(!VoidTDetector<A, A, A, A, A, A, A, A, A, B>::value);
}

namespace IsArrayTraits
{
    using StringLiteralType = __decltype("Hello");
    using NotArray = int;
    using Array = int[42];

    _Static_assert(ttl::is_array<StringLiteralType>::value);
    _Static_assert(ttl::is_array<Array>::value);
    _Static_assert(!ttl::is_array<NotArray>::value);

    _Static_assert(ttl::is_array_v<StringLiteralType>);
    _Static_assert(ttl::is_array_v<Array>);
    _Static_assert(!ttl::is_array_v<NotArray>);
}

namespace ArrayLenTests
{
    using StringLiteralType = __decltype(ttl::array_len("Hello"));
    using NotArray = __decltype(ttl::array_len(1));
    using Array = __decltype(ttl::array_len((int[42])0));

    template<typename T, uint ExpectedLength>
    struct Tester : ttl::integral_constant<bool, T::value == ExpectedLength>{};

    _Static_assert(Tester<StringLiteralType, 6u>::value);
    _Static_assert(Tester<Array, 42u>::value);
    _Static_assert(Tester<NotArray, 0u>::value);
}

namespace IsBaseOfTests
{
    struct A{};
    struct B : A{};
    struct C : B{};

    struct D{};

    _Static_assert(ttl::is_base_of<A, B>::value);
    _Static_assert(ttl::is_base_of<A, C>::value);
    _Static_assert(ttl::is_base_of<A, A>::value);

    _Static_assert(!ttl::is_base_of<int, int>::value);
    _Static_assert(!ttl::is_base_of<A, D>::value);
    _Static_assert(!ttl::is_base_of<int, A>::value);

    _Static_assert(ttl::is_base_of_v<A, B>);
    _Static_assert(ttl::is_base_of_v<A, C>);
    _Static_assert(ttl::is_base_of_v<A, A>);

    _Static_assert(!ttl::is_base_of_v<int, int>);
    _Static_assert(!ttl::is_base_of_v<A, D>);
    _Static_assert(!ttl::is_base_of_v<int, A>);
}

namespace AlwaysFalseTests
{
    struct A{};

    _Static_assert(!ttl::always_false<A>::value);
    _Static_assert(!ttl::always_false<int>::value);
    _Static_assert(!ttl::always_false<void>::value);
    _Static_assert(!ttl::always_false<float>::value);

    _Static_assert(!ttl::always_false_v<A>);
    _Static_assert(!ttl::always_false_v<int>);
    _Static_assert(!ttl::always_false_v<void>);
    _Static_assert(!ttl::always_false_v<float>);
}