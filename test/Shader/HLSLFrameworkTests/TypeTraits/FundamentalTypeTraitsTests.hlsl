#include "/Test/TTL/static_assert.hlsli"
#include "/Test/TTL/type_traits.hlsli"

struct MyStruct
{
    int a;
};

void ScalarTests()
{
    STATIC_ASSERT((ttl::fundamental_type_traits<bool>::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<int16_t>::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<int32_t>::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<int64_t>::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<uint16_t>::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<uint32_t>::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<uint64_t>::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<float16_t>::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<float32_t>::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<float64_t>::is_fundamental));
    STATIC_ASSERT((!ttl::fundamental_type_traits<MyStruct>::is_fundamental));

    STATIC_ASSERT((1u == ttl::fundamental_type_traits<bool>::dim0));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<int16_t>::dim0));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<int32_t>::dim0));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<int64_t>::dim0));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<uint16_t>::dim0));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<uint32_t>::dim0));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<uint64_t>::dim0));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<float16_t>::dim0));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<float32_t>::dim0));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<float64_t>::dim0));
    STATIC_ASSERT((0u == ttl::fundamental_type_traits<MyStruct>::dim0));

    STATIC_ASSERT((1u == ttl::fundamental_type_traits<bool>::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<int16_t>::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<int32_t>::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<int64_t>::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<uint16_t>::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<uint32_t>::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<uint64_t>::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<float16_t>::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<float32_t>::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<float64_t>::dim1));
    STATIC_ASSERT((0u == ttl::fundamental_type_traits<MyStruct>::dim1));

    STATIC_ASSERT((ttl::is_same<bool, ttl::fundamental_type_traits<bool>::base_type>::value));
    STATIC_ASSERT((ttl::is_same<int16_t, ttl::fundamental_type_traits<int16_t>::base_type>::value));
    STATIC_ASSERT((ttl::is_same<int32_t, ttl::fundamental_type_traits<int32_t>::base_type>::value));
    STATIC_ASSERT((ttl::is_same<int64_t, ttl::fundamental_type_traits<int64_t>::base_type>::value));
    STATIC_ASSERT((ttl::is_same<uint16_t, ttl::fundamental_type_traits<uint16_t>::base_type>::value));
    STATIC_ASSERT((ttl::is_same<uint32_t, ttl::fundamental_type_traits<uint32_t>::base_type>::value));
    STATIC_ASSERT((ttl::is_same<uint64_t, ttl::fundamental_type_traits<uint64_t>::base_type>::value));
    STATIC_ASSERT((ttl::is_same<float16_t, ttl::fundamental_type_traits<float16_t>::base_type>::value));
    STATIC_ASSERT((ttl::is_same<float32_t, ttl::fundamental_type_traits<float32_t>::base_type>::value));
    STATIC_ASSERT((ttl::is_same<float64_t, ttl::fundamental_type_traits<float64_t>::base_type>::value));
    STATIC_ASSERT((ttl::is_same<void, ttl::fundamental_type_traits<MyStruct>::base_type>::value));
}

template<uint InDim>
void VectorTests()
{
    STATIC_ASSERT((ttl::fundamental_type_traits<vector<bool, InDim> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<vector<int16_t, InDim> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<vector<int32_t, InDim> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<vector<int64_t, InDim> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<vector<uint16_t, InDim> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<vector<uint32_t, InDim> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<vector<uint64_t, InDim> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<vector<float16_t, InDim> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<vector<float32_t, InDim> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<vector<float64_t, InDim> >::is_fundamental));

    STATIC_ASSERT((InDim == ttl::fundamental_type_traits<vector<bool, InDim> >::dim0));
    STATIC_ASSERT((InDim == ttl::fundamental_type_traits<vector<int16_t, InDim> >::dim0));
    STATIC_ASSERT((InDim == ttl::fundamental_type_traits<vector<int32_t, InDim> >::dim0));
    STATIC_ASSERT((InDim == ttl::fundamental_type_traits<vector<int64_t, InDim> >::dim0));
    STATIC_ASSERT((InDim == ttl::fundamental_type_traits<vector<uint16_t, InDim> >::dim0));
    STATIC_ASSERT((InDim == ttl::fundamental_type_traits<vector<uint32_t, InDim> >::dim0));
    STATIC_ASSERT((InDim == ttl::fundamental_type_traits<vector<uint64_t, InDim> >::dim0));
    STATIC_ASSERT((InDim == ttl::fundamental_type_traits<vector<float16_t, InDim> >::dim0));
    STATIC_ASSERT((InDim == ttl::fundamental_type_traits<vector<float32_t, InDim> >::dim0));
    STATIC_ASSERT((InDim == ttl::fundamental_type_traits<vector<float64_t, InDim> >::dim0));

    STATIC_ASSERT((1u == ttl::fundamental_type_traits<vector<bool, InDim> >::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<vector<int16_t, InDim> >::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<vector<int32_t, InDim> >::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<vector<int64_t, InDim> >::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<vector<uint16_t, InDim> >::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<vector<uint32_t, InDim> >::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<vector<uint64_t, InDim> >::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<vector<float16_t, InDim> >::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<vector<float32_t, InDim> >::dim1));
    STATIC_ASSERT((1u == ttl::fundamental_type_traits<vector<float64_t, InDim> >::dim1));

    STATIC_ASSERT((ttl::is_same<bool, typename ttl::fundamental_type_traits<vector<bool, InDim> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<int16_t, typename ttl::fundamental_type_traits<vector<int16_t, InDim> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<int32_t, typename ttl::fundamental_type_traits<vector<int32_t, InDim> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<int64_t, typename ttl::fundamental_type_traits<vector<int64_t, InDim> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<uint16_t, typename ttl::fundamental_type_traits<vector<uint16_t, InDim> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<uint32_t, typename ttl::fundamental_type_traits<vector<uint32_t, InDim> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<uint64_t, typename ttl::fundamental_type_traits<vector<uint64_t, InDim> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<float16_t, typename ttl::fundamental_type_traits<vector<float16_t, InDim> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<float32_t, typename ttl::fundamental_type_traits<vector<float32_t, InDim> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<float64_t, typename ttl::fundamental_type_traits<vector<float64_t, InDim> >::base_type>::value));
}

template<uint InDim0, uint InDim1>
void MatrixTests()
{
    STATIC_ASSERT((ttl::fundamental_type_traits<matrix<bool, InDim0, InDim1> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<matrix<int16_t, InDim0, InDim1> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<matrix<int32_t, InDim0, InDim1> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<matrix<int64_t, InDim0, InDim1> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<matrix<uint16_t, InDim0, InDim1> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<matrix<uint32_t, InDim0, InDim1> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<matrix<uint64_t, InDim0, InDim1> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<matrix<float16_t, InDim0, InDim1> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<matrix<float32_t, InDim0, InDim1> >::is_fundamental));
    STATIC_ASSERT((ttl::fundamental_type_traits<matrix<float64_t, InDim0, InDim1> >::is_fundamental));

    STATIC_ASSERT((InDim0 == ttl::fundamental_type_traits<matrix<bool, InDim0, InDim1> >::dim0));
    STATIC_ASSERT((InDim0 == ttl::fundamental_type_traits<matrix<int16_t, InDim0, InDim1> >::dim0));
    STATIC_ASSERT((InDim0 == ttl::fundamental_type_traits<matrix<int32_t, InDim0, InDim1> >::dim0));
    STATIC_ASSERT((InDim0 == ttl::fundamental_type_traits<matrix<int64_t, InDim0, InDim1> >::dim0));
    STATIC_ASSERT((InDim0 == ttl::fundamental_type_traits<matrix<uint16_t, InDim0, InDim1> >::dim0));
    STATIC_ASSERT((InDim0 == ttl::fundamental_type_traits<matrix<uint32_t, InDim0, InDim1> >::dim0));
    STATIC_ASSERT((InDim0 == ttl::fundamental_type_traits<matrix<uint64_t, InDim0, InDim1> >::dim0));
    STATIC_ASSERT((InDim0 == ttl::fundamental_type_traits<matrix<float16_t, InDim0, InDim1> >::dim0));
    STATIC_ASSERT((InDim0 == ttl::fundamental_type_traits<matrix<float32_t, InDim0, InDim1> >::dim0));
    STATIC_ASSERT((InDim0 == ttl::fundamental_type_traits<matrix<float64_t, InDim0, InDim1> >::dim0));

    STATIC_ASSERT((InDim1 == ttl::fundamental_type_traits<matrix<bool, InDim0, InDim1> >::dim1));
    STATIC_ASSERT((InDim1 == ttl::fundamental_type_traits<matrix<int16_t, InDim0, InDim1> >::dim1));
    STATIC_ASSERT((InDim1 == ttl::fundamental_type_traits<matrix<int32_t, InDim0, InDim1> >::dim1));
    STATIC_ASSERT((InDim1 == ttl::fundamental_type_traits<matrix<int64_t, InDim0, InDim1> >::dim1));
    STATIC_ASSERT((InDim1 == ttl::fundamental_type_traits<matrix<uint16_t, InDim0, InDim1> >::dim1));
    STATIC_ASSERT((InDim1 == ttl::fundamental_type_traits<matrix<uint32_t, InDim0, InDim1> >::dim1));
    STATIC_ASSERT((InDim1 == ttl::fundamental_type_traits<matrix<uint64_t, InDim0, InDim1> >::dim1));
    STATIC_ASSERT((InDim1 == ttl::fundamental_type_traits<matrix<float16_t, InDim0, InDim1> >::dim1));
    STATIC_ASSERT((InDim1 == ttl::fundamental_type_traits<matrix<float32_t, InDim0, InDim1> >::dim1));
    STATIC_ASSERT((InDim1 == ttl::fundamental_type_traits<matrix<float64_t, InDim0, InDim1> >::dim1));

    STATIC_ASSERT((ttl::is_same<bool, typename ttl::fundamental_type_traits<matrix<bool, InDim0, InDim1> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<int16_t, typename ttl::fundamental_type_traits<matrix<int16_t, InDim0, InDim1> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<int32_t, typename ttl::fundamental_type_traits<matrix<int32_t, InDim0, InDim1> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<int64_t, typename ttl::fundamental_type_traits<matrix<int64_t, InDim0, InDim1> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<uint16_t, typename ttl::fundamental_type_traits<matrix<uint16_t, InDim0, InDim1> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<uint32_t, typename ttl::fundamental_type_traits<matrix<uint32_t, InDim0, InDim1> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<uint64_t, typename ttl::fundamental_type_traits<matrix<uint64_t, InDim0, InDim1> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<float16_t, typename ttl::fundamental_type_traits<matrix<float16_t, InDim0, InDim1> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<float32_t, typename ttl::fundamental_type_traits<matrix<float32_t, InDim0, InDim1> >::base_type>::value));
    STATIC_ASSERT((ttl::is_same<float64_t, typename ttl::fundamental_type_traits<matrix<float64_t, InDim0, InDim1> >::base_type>::value));
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

[numthreads(1,1,1)]
void AllCompile()
{
}