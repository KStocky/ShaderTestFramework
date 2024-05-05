#ifndef STF_BYTE_READER_TRAITS_HEADER
#define STF_BYTE_READER_TRAITS_HEADER

#include "/Test/TTL/type_traits.hlsli"

namespace STFPrivate
{
    template<typename U>
    struct TypeToVal;

    template<> struct TypeToVal<bool> : ttl::integral_constant<uint, 0>{};
    template<> struct TypeToVal<int16_t> : ttl::integral_constant<uint, 1>{};
    template<> struct TypeToVal<int32_t> : ttl::integral_constant<uint, 1>{};
    template<> struct TypeToVal<int64_t> : ttl::integral_constant<uint, 1>{};

    template<> struct TypeToVal<uint16_t> : ttl::integral_constant<uint, 2>{};
    template<> struct TypeToVal<uint32_t> : ttl::integral_constant<uint, 2>{};
    template<> struct TypeToVal<uint64_t> : ttl::integral_constant<uint, 2>{};

    template<> struct TypeToVal<float16_t> : ttl::integral_constant<uint, 3>{};
    template<> struct TypeToVal<float32_t> : ttl::integral_constant<uint, 3>{};
    template<> struct TypeToVal<float64_t> : ttl::integral_constant<uint, 3>{};

    template<uint U>
    struct NumBitsToValBase;

    template<> struct NumBitsToValBase<2> : ttl::integral_constant<uint, 0>{};
    template<> struct NumBitsToValBase<4> : ttl::integral_constant<uint, 1>{};
    template<> struct NumBitsToValBase<8> : ttl::integral_constant<uint, 2>{};
}

namespace STF
{
    template<uint16_t InReaderId, uint16_t InTypeId = 0>
    struct ByteReaderTraitsBase
    {
        static const uint16_t ReaderId = InReaderId;
        static const uint16_t TypeId = InTypeId;
    };

    template<typename T, typename = void>
    struct ByteReaderTraits : ByteReaderTraitsBase<0, 0>
    {
    };

    template<typename T, typename = void>
    struct PackedFundamentalTypeInfo;

    template<typename T>
    struct PackedFundamentalTypeInfo<T, typename ttl::enable_if<ttl::fundamental_type_traits<T>::is_fundamental>::type>
    {
        using Traits = ttl::fundamental_type_traits<T>;

        template<typename U>
        struct NumBitsToVal : STFPrivate::NumBitsToValBase<ttl::size_of<U>::value>{};
        
        static const uint16_t TypeVal = STFPrivate::TypeToVal<typename Traits::base_type>::value & 3;
        static const uint16_t NumBitsVal = NumBitsToVal<typename Traits::base_type>::value & 3;
        static const uint16_t NumColumns = (Traits::dim0 - 1) & 3;
        static const uint16_t NumRows = (Traits::dim1 - 1) & 3;

        static const uint16_t PackedTypeVal = TypeVal;
        static const uint16_t PackedNumBitsVal = NumBitsVal << 2;
        static const uint16_t PackedNumColumns = NumColumns << 4;
        static const uint16_t PackedNumRows = NumRows << 6;
    };
}

namespace STF
{
    template<typename T>
    struct ByteReaderTraits<T, typename ttl::enable_if<ttl::fundamental_type_traits<T>::is_fundamental>::type>
    {
        using PackedInfo = PackedFundamentalTypeInfo<T>;
        static const uint16_t ReaderId = READER_ID_FUNDAMENTAL;
        static const uint16_t TypeId = PackedInfo::PackedNumRows | PackedInfo::PackedNumColumns | PackedInfo::PackedNumBitsVal | PackedInfo::PackedTypeVal;
    };
}

#endif
