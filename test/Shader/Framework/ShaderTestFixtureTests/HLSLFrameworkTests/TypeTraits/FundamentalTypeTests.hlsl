#include "/Test/Public/ShaderTestFramework.hlsli"

struct MyStruct
{
    int a;
};

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void AllShouldPass()
{
    STF::IsTrue(STF::is_fundamental<bool>::value);
    STF::IsTrue(STF::is_fundamental<int>::value);
    STF::IsTrue(STF::is_fundamental<int2>::value);
    STF::IsTrue(STF::is_fundamental<int3>::value);
    STF::IsTrue(STF::is_fundamental<int4>::value);
    STF::IsTrue(STF::is_fundamental<uint>::value);
    STF::IsTrue(STF::is_fundamental<uint2>::value);
    STF::IsTrue(STF::is_fundamental<uint3>::value);
    STF::IsTrue(STF::is_fundamental<uint4>::value);
    STF::IsTrue(STF::is_fundamental<float>::value);
    STF::IsTrue(STF::is_fundamental<float2>::value);
    STF::IsTrue(STF::is_fundamental<float3>::value);
    STF::IsTrue(STF::is_fundamental<float4>::value);
    STF::IsFalse(STF::is_fundamental<MyStruct>::value);
}