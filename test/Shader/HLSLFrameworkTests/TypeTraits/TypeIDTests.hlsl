#include "/Test/TTL/static_assert.hlsli"
#include "/Test/STF/TypeId.hlsli"

[numthreads(1, 1, 1)]
void NoDuplicateTypeIDs()
{
    STATIC_ASSERT((STF::type_id<bool>::value != STF::type_id<uint>::value));
    STATIC_ASSERT((STF::type_id<bool>::value != STF::type_id<uint2>::value));
    STATIC_ASSERT((STF::type_id<bool>::value != STF::type_id<uint3>::value));
    STATIC_ASSERT((STF::type_id<bool>::value != STF::type_id<uint4>::value));
    STATIC_ASSERT((STF::type_id<bool>::value != STF::type_id<int>::value));
    STATIC_ASSERT((STF::type_id<bool>::value != STF::type_id<int2>::value));
    STATIC_ASSERT((STF::type_id<bool>::value != STF::type_id<int3>::value));
    STATIC_ASSERT((STF::type_id<bool>::value != STF::type_id<int4>::value));
    STATIC_ASSERT((STF::type_id<bool>::value != STF::type_id<float>::value));
    STATIC_ASSERT((STF::type_id<bool>::value != STF::type_id<float2>::value));
    STATIC_ASSERT((STF::type_id<bool>::value != STF::type_id<float3>::value));
    STATIC_ASSERT((STF::type_id<bool>::value != STF::type_id<float4>::value));

    STATIC_ASSERT((STF::type_id<uint>::value != STF::type_id<uint2>::value));
    STATIC_ASSERT((STF::type_id<uint>::value != STF::type_id<uint3>::value));
    STATIC_ASSERT((STF::type_id<uint>::value != STF::type_id<uint4>::value));
    STATIC_ASSERT((STF::type_id<uint>::value != STF::type_id<int>::value));
    STATIC_ASSERT((STF::type_id<uint>::value != STF::type_id<int2>::value));
    STATIC_ASSERT((STF::type_id<uint>::value != STF::type_id<int3>::value));
    STATIC_ASSERT((STF::type_id<uint>::value != STF::type_id<int4>::value));
    STATIC_ASSERT((STF::type_id<uint>::value != STF::type_id<float>::value));
    STATIC_ASSERT((STF::type_id<uint>::value != STF::type_id<float2>::value));
    STATIC_ASSERT((STF::type_id<uint>::value != STF::type_id<float3>::value));
    STATIC_ASSERT((STF::type_id<uint>::value != STF::type_id<float4>::value));

    STATIC_ASSERT((STF::type_id<uint2>::value != STF::type_id<uint3>::value));
    STATIC_ASSERT((STF::type_id<uint2>::value != STF::type_id<uint4>::value));
    STATIC_ASSERT((STF::type_id<uint2>::value != STF::type_id<int>::value));
    STATIC_ASSERT((STF::type_id<uint2>::value != STF::type_id<int2>::value));
    STATIC_ASSERT((STF::type_id<uint2>::value != STF::type_id<int3>::value));
    STATIC_ASSERT((STF::type_id<uint2>::value != STF::type_id<int4>::value));
    STATIC_ASSERT((STF::type_id<uint2>::value != STF::type_id<float>::value));
    STATIC_ASSERT((STF::type_id<uint2>::value != STF::type_id<float2>::value));
    STATIC_ASSERT((STF::type_id<uint2>::value != STF::type_id<float3>::value));
    STATIC_ASSERT((STF::type_id<uint2>::value != STF::type_id<float4>::value));

    STATIC_ASSERT((STF::type_id<uint3>::value != STF::type_id<uint4>::value));
    STATIC_ASSERT((STF::type_id<uint3>::value != STF::type_id<int>::value));
    STATIC_ASSERT((STF::type_id<uint3>::value != STF::type_id<int2>::value));
    STATIC_ASSERT((STF::type_id<uint3>::value != STF::type_id<int3>::value));
    STATIC_ASSERT((STF::type_id<uint3>::value != STF::type_id<int4>::value));
    STATIC_ASSERT((STF::type_id<uint3>::value != STF::type_id<float>::value));
    STATIC_ASSERT((STF::type_id<uint3>::value != STF::type_id<float2>::value));
    STATIC_ASSERT((STF::type_id<uint3>::value != STF::type_id<float3>::value));
    STATIC_ASSERT((STF::type_id<uint3>::value != STF::type_id<float4>::value));

    STATIC_ASSERT((STF::type_id<uint4>::value != STF::type_id<int>::value));
    STATIC_ASSERT((STF::type_id<uint4>::value != STF::type_id<int2>::value));
    STATIC_ASSERT((STF::type_id<uint4>::value != STF::type_id<int3>::value));
    STATIC_ASSERT((STF::type_id<uint4>::value != STF::type_id<int4>::value));
    STATIC_ASSERT((STF::type_id<uint4>::value != STF::type_id<float>::value));
    STATIC_ASSERT((STF::type_id<uint4>::value != STF::type_id<float2>::value));
    STATIC_ASSERT((STF::type_id<uint4>::value != STF::type_id<float3>::value));
    STATIC_ASSERT((STF::type_id<uint4>::value != STF::type_id<float4>::value));

    STATIC_ASSERT((STF::type_id<int>::value != STF::type_id<int2>::value));
    STATIC_ASSERT((STF::type_id<int>::value != STF::type_id<int3>::value));
    STATIC_ASSERT((STF::type_id<int>::value != STF::type_id<int4>::value));
    STATIC_ASSERT((STF::type_id<int>::value != STF::type_id<float>::value));
    STATIC_ASSERT((STF::type_id<int>::value != STF::type_id<float2>::value));
    STATIC_ASSERT((STF::type_id<int>::value != STF::type_id<float3>::value));
    STATIC_ASSERT((STF::type_id<int>::value != STF::type_id<float4>::value));

    STATIC_ASSERT((STF::type_id<int2>::value != STF::type_id<int3>::value));
    STATIC_ASSERT((STF::type_id<int2>::value != STF::type_id<int4>::value));
    STATIC_ASSERT((STF::type_id<int2>::value != STF::type_id<float>::value));
    STATIC_ASSERT((STF::type_id<int2>::value != STF::type_id<float2>::value));
    STATIC_ASSERT((STF::type_id<int2>::value != STF::type_id<float3>::value));
    STATIC_ASSERT((STF::type_id<int2>::value != STF::type_id<float4>::value));

    STATIC_ASSERT((STF::type_id<int3>::value != STF::type_id<int4>::value));
    STATIC_ASSERT((STF::type_id<int3>::value != STF::type_id<float>::value));
    STATIC_ASSERT((STF::type_id<int3>::value != STF::type_id<float2>::value));
    STATIC_ASSERT((STF::type_id<int3>::value != STF::type_id<float3>::value));
    STATIC_ASSERT((STF::type_id<int3>::value != STF::type_id<float4>::value));

    STATIC_ASSERT((STF::type_id<int4>::value != STF::type_id<float>::value));
    STATIC_ASSERT((STF::type_id<int4>::value != STF::type_id<float2>::value));
    STATIC_ASSERT((STF::type_id<int4>::value != STF::type_id<float3>::value));
    STATIC_ASSERT((STF::type_id<int4>::value != STF::type_id<float4>::value));

    STATIC_ASSERT((STF::type_id<float>::value != STF::type_id<float2>::value));
    STATIC_ASSERT((STF::type_id<float>::value != STF::type_id<float3>::value));
    STATIC_ASSERT((STF::type_id<float>::value != STF::type_id<float4>::value));

    STATIC_ASSERT((STF::type_id<float2>::value != STF::type_id<float3>::value));
    STATIC_ASSERT((STF::type_id<float2>::value != STF::type_id<float4>::value));

    STATIC_ASSERT((STF::type_id<float3>::value != STF::type_id<float4>::value));
}