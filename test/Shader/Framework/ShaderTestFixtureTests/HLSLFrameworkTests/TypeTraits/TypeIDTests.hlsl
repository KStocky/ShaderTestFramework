#include "/Test/Public/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void NoDuplicateTypeIDs()
{
    STF::NotEqual(STF::type_id<bool>::value, STF::type_id<uint>::value);
    STF::NotEqual(STF::type_id<bool>::value, STF::type_id<uint2>::value);
    STF::NotEqual(STF::type_id<bool>::value, STF::type_id<uint3>::value);
    STF::NotEqual(STF::type_id<bool>::value, STF::type_id<uint4>::value);
    STF::NotEqual(STF::type_id<bool>::value, STF::type_id<int>::value);
    STF::NotEqual(STF::type_id<bool>::value, STF::type_id<int2>::value);
    STF::NotEqual(STF::type_id<bool>::value, STF::type_id<int3>::value);
    STF::NotEqual(STF::type_id<bool>::value, STF::type_id<int4>::value);
    STF::NotEqual(STF::type_id<bool>::value, STF::type_id<float>::value);
    STF::NotEqual(STF::type_id<bool>::value, STF::type_id<float2>::value);
    STF::NotEqual(STF::type_id<bool>::value, STF::type_id<float3>::value);
    STF::NotEqual(STF::type_id<bool>::value, STF::type_id<float4>::value);

    STF::NotEqual(STF::type_id<uint>::value, STF::type_id<uint2>::value);
    STF::NotEqual(STF::type_id<uint>::value, STF::type_id<uint3>::value);
    STF::NotEqual(STF::type_id<uint>::value, STF::type_id<uint4>::value);
    STF::NotEqual(STF::type_id<uint>::value, STF::type_id<int>::value);
    STF::NotEqual(STF::type_id<uint>::value, STF::type_id<int2>::value);
    STF::NotEqual(STF::type_id<uint>::value, STF::type_id<int3>::value);
    STF::NotEqual(STF::type_id<uint>::value, STF::type_id<int4>::value);
    STF::NotEqual(STF::type_id<uint>::value, STF::type_id<float>::value);
    STF::NotEqual(STF::type_id<uint>::value, STF::type_id<float2>::value);
    STF::NotEqual(STF::type_id<uint>::value, STF::type_id<float3>::value);
    STF::NotEqual(STF::type_id<uint>::value, STF::type_id<float4>::value);

    STF::NotEqual(STF::type_id<uint2>::value, STF::type_id<uint3>::value);
    STF::NotEqual(STF::type_id<uint2>::value, STF::type_id<uint4>::value);
    STF::NotEqual(STF::type_id<uint2>::value, STF::type_id<int>::value);
    STF::NotEqual(STF::type_id<uint2>::value, STF::type_id<int2>::value);
    STF::NotEqual(STF::type_id<uint2>::value, STF::type_id<int3>::value);
    STF::NotEqual(STF::type_id<uint2>::value, STF::type_id<int4>::value);
    STF::NotEqual(STF::type_id<uint2>::value, STF::type_id<float>::value);
    STF::NotEqual(STF::type_id<uint2>::value, STF::type_id<float2>::value);
    STF::NotEqual(STF::type_id<uint2>::value, STF::type_id<float3>::value);
    STF::NotEqual(STF::type_id<uint2>::value, STF::type_id<float4>::value);

    STF::NotEqual(STF::type_id<uint3>::value, STF::type_id<uint4>::value);
    STF::NotEqual(STF::type_id<uint3>::value, STF::type_id<int>::value);
    STF::NotEqual(STF::type_id<uint3>::value, STF::type_id<int2>::value);
    STF::NotEqual(STF::type_id<uint3>::value, STF::type_id<int3>::value);
    STF::NotEqual(STF::type_id<uint3>::value, STF::type_id<int4>::value);
    STF::NotEqual(STF::type_id<uint3>::value, STF::type_id<float>::value);
    STF::NotEqual(STF::type_id<uint3>::value, STF::type_id<float2>::value);
    STF::NotEqual(STF::type_id<uint3>::value, STF::type_id<float3>::value);
    STF::NotEqual(STF::type_id<uint3>::value, STF::type_id<float4>::value);

    STF::NotEqual(STF::type_id<uint4>::value, STF::type_id<int>::value);
    STF::NotEqual(STF::type_id<uint4>::value, STF::type_id<int2>::value);
    STF::NotEqual(STF::type_id<uint4>::value, STF::type_id<int3>::value);
    STF::NotEqual(STF::type_id<uint4>::value, STF::type_id<int4>::value);
    STF::NotEqual(STF::type_id<uint4>::value, STF::type_id<float>::value);
    STF::NotEqual(STF::type_id<uint4>::value, STF::type_id<float2>::value);
    STF::NotEqual(STF::type_id<uint4>::value, STF::type_id<float3>::value);
    STF::NotEqual(STF::type_id<uint4>::value, STF::type_id<float4>::value);

    STF::NotEqual(STF::type_id<int>::value, STF::type_id<int2>::value);
    STF::NotEqual(STF::type_id<int>::value, STF::type_id<int3>::value);
    STF::NotEqual(STF::type_id<int>::value, STF::type_id<int4>::value);
    STF::NotEqual(STF::type_id<int>::value, STF::type_id<float>::value);
    STF::NotEqual(STF::type_id<int>::value, STF::type_id<float2>::value);
    STF::NotEqual(STF::type_id<int>::value, STF::type_id<float3>::value);
    STF::NotEqual(STF::type_id<int>::value, STF::type_id<float4>::value);

    STF::NotEqual(STF::type_id<int2>::value, STF::type_id<int3>::value);
    STF::NotEqual(STF::type_id<int2>::value, STF::type_id<int4>::value);
    STF::NotEqual(STF::type_id<int2>::value, STF::type_id<float>::value);
    STF::NotEqual(STF::type_id<int2>::value, STF::type_id<float2>::value);
    STF::NotEqual(STF::type_id<int2>::value, STF::type_id<float3>::value);
    STF::NotEqual(STF::type_id<int2>::value, STF::type_id<float4>::value);

    STF::NotEqual(STF::type_id<int3>::value, STF::type_id<int4>::value);
    STF::NotEqual(STF::type_id<int3>::value, STF::type_id<float>::value);
    STF::NotEqual(STF::type_id<int3>::value, STF::type_id<float2>::value);
    STF::NotEqual(STF::type_id<int3>::value, STF::type_id<float3>::value);
    STF::NotEqual(STF::type_id<int3>::value, STF::type_id<float4>::value);

    STF::NotEqual(STF::type_id<int4>::value, STF::type_id<float>::value);
    STF::NotEqual(STF::type_id<int4>::value, STF::type_id<float2>::value);
    STF::NotEqual(STF::type_id<int4>::value, STF::type_id<float3>::value);
    STF::NotEqual(STF::type_id<int4>::value, STF::type_id<float4>::value);

    STF::NotEqual(STF::type_id<float>::value, STF::type_id<float2>::value);
    STF::NotEqual(STF::type_id<float>::value, STF::type_id<float3>::value);
    STF::NotEqual(STF::type_id<float>::value, STF::type_id<float4>::value);

    STF::NotEqual(STF::type_id<float2>::value, STF::type_id<float3>::value);
    STF::NotEqual(STF::type_id<float2>::value, STF::type_id<float4>::value);

    STF::NotEqual(STF::type_id<float3>::value, STF::type_id<float4>::value);
}