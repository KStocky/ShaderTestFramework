#pragma once

#include "/Test/TTL/type_traits.hlsli"

namespace STF
{
    template<typename T>
    struct type_id : ttl::integral_constant<uint, 0>
    {};

    template<> struct type_id<bool> : ttl::integral_constant<uint, TYPE_ID_BOOL>{};

    template<> struct type_id<int> : ttl::integral_constant<uint, TYPE_ID_INT>{};
    template<> struct type_id<int2> : ttl::integral_constant<uint, TYPE_ID_INT2>{};
    template<> struct type_id<int3> : ttl::integral_constant<uint, TYPE_ID_INT3>{};
    template<> struct type_id<int4> : ttl::integral_constant<uint, TYPE_ID_INT4>{};

    template<> struct type_id<uint> : ttl::integral_constant<uint, TYPE_ID_UINT>{};
    template<> struct type_id<uint2> : ttl::integral_constant<uint, TYPE_ID_UINT2>{};
    template<> struct type_id<uint3> : ttl::integral_constant<uint, TYPE_ID_UINT3>{};
    template<> struct type_id<uint4> : ttl::integral_constant<uint, TYPE_ID_UINT4>{};

    template<> struct type_id<float> : ttl::integral_constant<uint, TYPE_ID_FLOAT>{};
    template<> struct type_id<float2> : ttl::integral_constant<uint, TYPE_ID_FLOAT2>{};
    template<> struct type_id<float3> : ttl::integral_constant<uint, TYPE_ID_FLOAT3>{};
    template<> struct type_id<float4> : ttl::integral_constant<uint, TYPE_ID_FLOAT4>{};
}