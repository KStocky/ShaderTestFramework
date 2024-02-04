#pragma once

namespace STF
{
    template<uint16_t InReaderId, uint16_t InTypeId = 0>
    struct ByteReaderTraitsBase
    {
        static const uint16_t ReaderId = InReaderId;
        static const uint16_t TypeId = InTypeId;
    };

    template<typename T>
    struct ByteReaderTraits : ByteReaderTraitsBase<0, 0>
    {
    };

    template<> struct ByteReaderTraits<bool> : ByteReaderTraitsBase<TYPE_ID_BOOL>{};
    template<> struct ByteReaderTraits<bool2> : ByteReaderTraitsBase<TYPE_ID_BOOL2>{};
    template<> struct ByteReaderTraits<bool3> : ByteReaderTraitsBase<TYPE_ID_BOOL3>{};
    template<> struct ByteReaderTraits<bool4> : ByteReaderTraitsBase<TYPE_ID_BOOL4>{};
    
    template<> struct ByteReaderTraits<int16_t> : ByteReaderTraitsBase<TYPE_ID_INT16>{};
    template<> struct ByteReaderTraits<int16_t2> : ByteReaderTraitsBase<TYPE_ID_INT16_2>{};
    template<> struct ByteReaderTraits<int16_t3> : ByteReaderTraitsBase<TYPE_ID_INT16_3>{};
    template<> struct ByteReaderTraits<int16_t4> : ByteReaderTraitsBase<TYPE_ID_INT16_4>{};

    template<> struct ByteReaderTraits<int> : ByteReaderTraitsBase<TYPE_ID_INT>{};
    template<> struct ByteReaderTraits<int2> : ByteReaderTraitsBase<TYPE_ID_INT2>{};
    template<> struct ByteReaderTraits<int3> : ByteReaderTraitsBase<TYPE_ID_INT3>{};
    template<> struct ByteReaderTraits<int4> : ByteReaderTraitsBase<TYPE_ID_INT4>{};
    
    template<> struct ByteReaderTraits<int64_t> : ByteReaderTraitsBase<TYPE_ID_INT64>{};
    template<> struct ByteReaderTraits<int64_t2> : ByteReaderTraitsBase<TYPE_ID_INT64_2>{};
    template<> struct ByteReaderTraits<int64_t3> : ByteReaderTraitsBase<TYPE_ID_INT64_3>{};
    template<> struct ByteReaderTraits<int64_t4> : ByteReaderTraitsBase<TYPE_ID_INT64_4>{};
    
    template<> struct ByteReaderTraits<uint16_t> : ByteReaderTraitsBase<TYPE_ID_UINT16>{};
    template<> struct ByteReaderTraits<uint16_t2> : ByteReaderTraitsBase<TYPE_ID_UINT16_2>{};
    template<> struct ByteReaderTraits<uint16_t3> : ByteReaderTraitsBase<TYPE_ID_UINT16_3>{};
    template<> struct ByteReaderTraits<uint16_t4> : ByteReaderTraitsBase<TYPE_ID_UINT16_4>{};

    template<> struct ByteReaderTraits<uint> : ByteReaderTraitsBase<TYPE_ID_UINT>{};
    template<> struct ByteReaderTraits<uint2> : ByteReaderTraitsBase<TYPE_ID_UINT2>{};
    template<> struct ByteReaderTraits<uint3> : ByteReaderTraitsBase<TYPE_ID_UINT3>{};
    template<> struct ByteReaderTraits<uint4> : ByteReaderTraitsBase<TYPE_ID_UINT4>{};
    
    template<> struct ByteReaderTraits<uint64_t> : ByteReaderTraitsBase<TYPE_ID_UINT64>{};
    template<> struct ByteReaderTraits<uint64_t2> : ByteReaderTraitsBase<TYPE_ID_UINT64_2>{};
    template<> struct ByteReaderTraits<uint64_t3> : ByteReaderTraitsBase<TYPE_ID_UINT64_3>{};
    template<> struct ByteReaderTraits<uint64_t4> : ByteReaderTraitsBase<TYPE_ID_UINT64_4>{};
    
    template<> struct ByteReaderTraits<float16_t> : ByteReaderTraitsBase<TYPE_ID_FLOAT16>{};
    template<> struct ByteReaderTraits<float16_t2> : ByteReaderTraitsBase<TYPE_ID_FLOAT16_2>{};
    template<> struct ByteReaderTraits<float16_t3> : ByteReaderTraitsBase<TYPE_ID_FLOAT16_3>{};
    template<> struct ByteReaderTraits<float16_t4> : ByteReaderTraitsBase<TYPE_ID_FLOAT16_4>{};

    template<> struct ByteReaderTraits<float> : ByteReaderTraitsBase<TYPE_ID_FLOAT>{};
    template<> struct ByteReaderTraits<float2> : ByteReaderTraitsBase<TYPE_ID_FLOAT2>{};
    template<> struct ByteReaderTraits<float3> : ByteReaderTraitsBase<TYPE_ID_FLOAT3>{};
    template<> struct ByteReaderTraits<float4> : ByteReaderTraitsBase<TYPE_ID_FLOAT4>{};
    
    template<> struct ByteReaderTraits<float64_t> : ByteReaderTraitsBase<TYPE_ID_FLOAT64>{};
    template<> struct ByteReaderTraits<float64_t2> : ByteReaderTraitsBase<TYPE_ID_FLOAT64_2>{};
    template<> struct ByteReaderTraits<float64_t3> : ByteReaderTraitsBase<TYPE_ID_FLOAT64_3>{};
    template<> struct ByteReaderTraits<float64_t4> : ByteReaderTraitsBase<TYPE_ID_FLOAT64_4>{};
}