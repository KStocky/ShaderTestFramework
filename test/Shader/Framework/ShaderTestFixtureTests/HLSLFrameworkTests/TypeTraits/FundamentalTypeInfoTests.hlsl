#include "/Test/Public/ShaderTestFramework.hlsli"


[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void AllShouldPass()
{
    STF::AreEqual(1u, STF::fundamental_type_info<bool>::rank);
    STF::AreEqual(1u, STF::fundamental_type_info<int>::rank);
    STF::AreEqual(2u, STF::fundamental_type_info<int2>::rank);
    STF::AreEqual(3u, STF::fundamental_type_info<int3>::rank);
    STF::AreEqual(4u, STF::fundamental_type_info<int4>::rank);
    STF::AreEqual(1u, STF::fundamental_type_info<uint>::rank);
    STF::AreEqual(2u, STF::fundamental_type_info<uint2>::rank);
    STF::AreEqual(3u, STF::fundamental_type_info<uint3>::rank);
    STF::AreEqual(4u, STF::fundamental_type_info<uint4>::rank);
    STF::AreEqual(1u, STF::fundamental_type_info<float>::rank);
    STF::AreEqual(2u, STF::fundamental_type_info<float2>::rank);
    STF::AreEqual(3u, STF::fundamental_type_info<float3>::rank);
    STF::AreEqual(4u, STF::fundamental_type_info<float4>::rank);

    STF::IsTrue(STF::is_same<bool, STF::fundamental_type_info<bool>::base_type>::value);
    STF::IsTrue(STF::is_same<int, STF::fundamental_type_info<int>::base_type>::value);
    STF::IsTrue(STF::is_same<int, STF::fundamental_type_info<int2>::base_type>::value);
    STF::IsTrue(STF::is_same<int, STF::fundamental_type_info<int3>::base_type>::value);
    STF::IsTrue(STF::is_same<int, STF::fundamental_type_info<int4>::base_type>::value);
    STF::IsTrue(STF::is_same<uint, STF::fundamental_type_info<uint>::base_type>::value);
    STF::IsTrue(STF::is_same<uint, STF::fundamental_type_info<uint2>::base_type>::value);
    STF::IsTrue(STF::is_same<uint, STF::fundamental_type_info<uint3>::base_type>::value);
    STF::IsTrue(STF::is_same<uint, STF::fundamental_type_info<uint4>::base_type>::value);
    STF::IsTrue(STF::is_same<float, STF::fundamental_type_info<float>::base_type>::value);
    STF::IsTrue(STF::is_same<float, STF::fundamental_type_info<float2>::base_type>::value);
    STF::IsTrue(STF::is_same<float, STF::fundamental_type_info<float3>::base_type>::value);
    STF::IsTrue(STF::is_same<float, STF::fundamental_type_info<float4>::base_type>::value);
}