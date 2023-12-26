#include "/Test/STF/ShaderTestFramework.hlsli"

struct MyStruct
{
    int a;
};

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void AllShouldPass()
{
    STF::IsTrue(ttl::is_fundamental<bool>::value);
    STF::IsTrue(ttl::is_fundamental<int>::value);
    STF::IsTrue(ttl::is_fundamental<int2>::value);
    STF::IsTrue(ttl::is_fundamental<int3>::value);
    STF::IsTrue(ttl::is_fundamental<int4>::value);
    STF::IsTrue(ttl::is_fundamental<uint>::value);
    STF::IsTrue(ttl::is_fundamental<uint2>::value);
    STF::IsTrue(ttl::is_fundamental<uint3>::value);
    STF::IsTrue(ttl::is_fundamental<uint4>::value);
    STF::IsTrue(ttl::is_fundamental<float>::value);
    STF::IsTrue(ttl::is_fundamental<float2>::value);
    STF::IsTrue(ttl::is_fundamental<float3>::value);
    STF::IsTrue(ttl::is_fundamental<float4>::value);
    STF::IsFalse(ttl::is_fundamental<MyStruct>::value);
}