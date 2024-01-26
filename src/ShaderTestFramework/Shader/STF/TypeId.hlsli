#pragma once

#include "/Test/TTL/type_traits.hlsli"

namespace STF
{
    template<typename T>
    struct type_id : ttl::integral_constant<uint, 0>
    {};

    template<> struct type_id<bool> : ttl::integral_constant<uint, TYPE_ID_BOOL>{};
    template<> struct type_id<bool2> : ttl::integral_constant<uint, TYPE_ID_BOOL2>{};
    template<> struct type_id<bool3> : ttl::integral_constant<uint, TYPE_ID_BOOL3>{};
    template<> struct type_id<bool4> : ttl::integral_constant<uint, TYPE_ID_BOOL4>{};
    
    template<> struct type_id<int16_t> : ttl::integral_constant<uint, TYPE_ID_INT16>{};
    template<> struct type_id<int16_t2> : ttl::integral_constant<uint, TYPE_ID_INT16_2>{};
    template<> struct type_id<int16_t3> : ttl::integral_constant<uint, TYPE_ID_INT16_3>{};
    template<> struct type_id<int16_t4> : ttl::integral_constant<uint, TYPE_ID_INT16_4>{};

    template<> struct type_id<int> : ttl::integral_constant<uint, TYPE_ID_INT>{};
    template<> struct type_id<int2> : ttl::integral_constant<uint, TYPE_ID_INT2>{};
    template<> struct type_id<int3> : ttl::integral_constant<uint, TYPE_ID_INT3>{};
    template<> struct type_id<int4> : ttl::integral_constant<uint, TYPE_ID_INT4>{};
    
    template<> struct type_id<int64_t> : ttl::integral_constant<uint, TYPE_ID_INT64>{};
    template<> struct type_id<int64_t2> : ttl::integral_constant<uint, TYPE_ID_INT64_2>{};
    template<> struct type_id<int64_t3> : ttl::integral_constant<uint, TYPE_ID_INT64_3>{};
    template<> struct type_id<int64_t4> : ttl::integral_constant<uint, TYPE_ID_INT64_4>{};
    
    template<> struct type_id<uint16_t> : ttl::integral_constant<uint, TYPE_ID_UINT16>{};
    template<> struct type_id<uint16_t2> : ttl::integral_constant<uint, TYPE_ID_UINT16_2>{};
    template<> struct type_id<uint16_t3> : ttl::integral_constant<uint, TYPE_ID_UINT16_3>{};
    template<> struct type_id<uint16_t4> : ttl::integral_constant<uint, TYPE_ID_UINT16_4>{};

    template<> struct type_id<uint> : ttl::integral_constant<uint, TYPE_ID_UINT>{};
    template<> struct type_id<uint2> : ttl::integral_constant<uint, TYPE_ID_UINT2>{};
    template<> struct type_id<uint3> : ttl::integral_constant<uint, TYPE_ID_UINT3>{};
    template<> struct type_id<uint4> : ttl::integral_constant<uint, TYPE_ID_UINT4>{};
    
    template<> struct type_id<uint64_t> : ttl::integral_constant<uint, TYPE_ID_UINT64>{};
    template<> struct type_id<uint64_t2> : ttl::integral_constant<uint, TYPE_ID_UINT64_2>{};
    template<> struct type_id<uint64_t3> : ttl::integral_constant<uint, TYPE_ID_UINT64_3>{};
    template<> struct type_id<uint64_t4> : ttl::integral_constant<uint, TYPE_ID_UINT64_4>{};
    
    template<> struct type_id<float16_t> : ttl::integral_constant<uint, TYPE_ID_FLOAT16>{};
    template<> struct type_id<float16_t2> : ttl::integral_constant<uint, TYPE_ID_FLOAT16_2>{};
    template<> struct type_id<float16_t3> : ttl::integral_constant<uint, TYPE_ID_FLOAT16_3>{};
    template<> struct type_id<float16_t4> : ttl::integral_constant<uint, TYPE_ID_FLOAT16_4>{};

    template<> struct type_id<float> : ttl::integral_constant<uint, TYPE_ID_FLOAT>{};
    template<> struct type_id<float2> : ttl::integral_constant<uint, TYPE_ID_FLOAT2>{};
    template<> struct type_id<float3> : ttl::integral_constant<uint, TYPE_ID_FLOAT3>{};
    template<> struct type_id<float4> : ttl::integral_constant<uint, TYPE_ID_FLOAT4>{};
    
    template<> struct type_id<float64_t> : ttl::integral_constant<uint, TYPE_ID_FLOAT64>{};
    template<> struct type_id<float64_t2> : ttl::integral_constant<uint, TYPE_ID_FLOAT64_2>{};
    template<> struct type_id<float64_t3> : ttl::integral_constant<uint, TYPE_ID_FLOAT64_3>{};
    template<> struct type_id<float64_t4> : ttl::integral_constant<uint, TYPE_ID_FLOAT64_4>{};
}