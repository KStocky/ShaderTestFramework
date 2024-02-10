#include "/Test/STF/ByteReaderTraits.hlsli"
#include "/Test/TTL/static_assert.hlsli"

namespace CustomByteReaderTraitsTests
{
    struct TypeWithoutReaderTraits
    {
    };

    struct TypeWithReaderTraitsNoTypeId
    {
        static const uint16_t ReaderId = 34;
    };

    struct TypeWithReaderTraitsAndTypeId
    {
        static const uint16_t ReaderId = 42;
        static const uint16_t TypeId = 124;
    }; 
}

namespace STF
{
    template<> struct ByteReaderTraits<CustomByteReaderTraitsTests::TypeWithReaderTraitsNoTypeId> 
        : ByteReaderTraitsBase<CustomByteReaderTraitsTests::TypeWithReaderTraitsNoTypeId::ReaderId>{};
    
    template<> struct ByteReaderTraits<CustomByteReaderTraitsTests::TypeWithReaderTraitsAndTypeId> 
        : ByteReaderTraitsBase<CustomByteReaderTraitsTests::TypeWithReaderTraitsAndTypeId::ReaderId, CustomByteReaderTraitsTests::TypeWithReaderTraitsAndTypeId::TypeId>{};
}

namespace CustomByteReaderTraitsTests
{
    STATIC_ASSERT(STF::ByteReaderTraits<TypeWithoutReaderTraits>::ReaderId == 0);
    STATIC_ASSERT(STF::ByteReaderTraits<TypeWithoutReaderTraits>::TypeId == 0);

    STATIC_ASSERT(STF::ByteReaderTraits<TypeWithReaderTraitsNoTypeId>::ReaderId == TypeWithReaderTraitsNoTypeId::ReaderId);
    STATIC_ASSERT(STF::ByteReaderTraits<TypeWithReaderTraitsNoTypeId>::TypeId == 0);

    STATIC_ASSERT(STF::ByteReaderTraits<TypeWithReaderTraitsAndTypeId>::ReaderId == TypeWithReaderTraitsAndTypeId::ReaderId);
    STATIC_ASSERT(STF::ByteReaderTraits<TypeWithReaderTraitsAndTypeId>::TypeId == TypeWithReaderTraitsAndTypeId::TypeId);
}

namespace PackedFundamentalTypeInfoTests
{
    template<typename T, uint ExpectedTypeVal, uint ExpectedNumBits, uint ExpectedNumColumns, uint ExpectedNumRows>
    void Test()
    {
        STATIC_ASSERT(STF::PackedFundamentalTypeInfo<T>::TypeVal == ExpectedTypeVal);
        STATIC_ASSERT(STF::PackedFundamentalTypeInfo<T>::PackedTypeVal == ExpectedTypeVal);

        STATIC_ASSERT(STF::PackedFundamentalTypeInfo<T>::NumBitsVal == ExpectedNumBits);
        STATIC_ASSERT((STF::PackedFundamentalTypeInfo<T>::PackedNumBitsVal >> 2) == ExpectedNumBits);

        STATIC_ASSERT(STF::PackedFundamentalTypeInfo<T>::NumColumns == ExpectedNumColumns);
        STATIC_ASSERT((STF::PackedFundamentalTypeInfo<T>::PackedNumColumns >> 4) == ExpectedNumColumns);

        STATIC_ASSERT(STF::PackedFundamentalTypeInfo<T>::NumRows == ExpectedNumRows);
        STATIC_ASSERT((STF::PackedFundamentalTypeInfo<T>::PackedNumRows >> 6) == ExpectedNumRows);
    }

    void ScalarTests()
    {
        Test<bool, 0, 1, 0, 0>();
        Test<int16_t, 1, 0, 0, 0>();
        Test<int32_t, 1, 1, 0, 0>();
        Test<int64_t, 1, 2, 0, 0>();

        Test<uint16_t, 2, 0, 0, 0>();
        Test<uint32_t, 2, 1, 0, 0>();
        Test<uint64_t, 2, 2, 0, 0>();

        Test<float16_t, 3, 0, 0, 0>();
        Test<float32_t, 3, 1, 0, 0>();
        Test<float64_t, 3, 2, 0, 0>();
    }

    template<uint InDim>
    void VectorTests()
    {
        Test<vector<bool, InDim>, 0, 1, InDim - 1, 0>();
        Test<vector<int16_t, InDim>, 1, 0, InDim - 1, 0>();
        Test<vector<int32_t, InDim>, 1, 1, InDim - 1, 0>();
        Test<vector<int64_t, InDim>, 1, 2, InDim - 1, 0>();

        Test<vector<uint16_t, InDim>, 2, 0, InDim - 1, 0>();
        Test<vector<uint32_t, InDim>, 2, 1, InDim - 1, 0>();
        Test<vector<uint64_t, InDim>, 2, 2, InDim - 1, 0>();

        Test<vector<float16_t, InDim>, 3, 0, InDim - 1, 0>();
        Test<vector<float32_t, InDim>, 3, 1, InDim - 1, 0>();
        Test<vector<float64_t, InDim>, 3, 2, InDim - 1, 0>();
    }

    template<uint InDim0, uint InDim1>
    void MatrixTests()
    {
        Test<matrix<bool, InDim0, InDim1>, 0, 1, InDim0 - 1, InDim1 - 1>();
        Test<matrix<int16_t, InDim0, InDim1>, 1, 0, InDim0 - 1, InDim1 - 1>();
        Test<matrix<int32_t, InDim0, InDim1>, 1, 1, InDim0 - 1, InDim1 - 1>();
        Test<matrix<int64_t, InDim0, InDim1>, 1, 2, InDim0 - 1, InDim1 - 1>();

        Test<matrix<uint16_t, InDim0, InDim1>, 2, 0, InDim0 - 1, InDim1 - 1>();
        Test<matrix<uint32_t, InDim0, InDim1>, 2, 1, InDim0 - 1, InDim1 - 1>();
        Test<matrix<uint64_t, InDim0, InDim1>, 2, 2, InDim0 - 1, InDim1 - 1>();

        Test<matrix<float16_t, InDim0, InDim1>, 3, 0, InDim0 - 1, InDim1 - 1>();
        Test<matrix<float32_t, InDim0, InDim1>, 3, 1, InDim0 - 1, InDim1 - 1>();
        Test<matrix<float64_t, InDim0, InDim1>, 3, 2, InDim0 - 1, InDim1 - 1>();
    }

    void RunTests()
    {
        ScalarTests();
        VectorTests<1>();
        VectorTests<2>();
        VectorTests<3>();
        VectorTests<4>();

        MatrixTests<1,1>();
        MatrixTests<1,2>();
        MatrixTests<1,3>();
        MatrixTests<1,4>();

        MatrixTests<2,1>();
        MatrixTests<2,2>();
        MatrixTests<2,3>();
        MatrixTests<2,4>();

        MatrixTests<3,1>();
        MatrixTests<3,2>();
        MatrixTests<3,3>();
        MatrixTests<3,4>();

        MatrixTests<4,1>();
        MatrixTests<4,2>();
        MatrixTests<4,3>();
        MatrixTests<4,4>();
    }
}
