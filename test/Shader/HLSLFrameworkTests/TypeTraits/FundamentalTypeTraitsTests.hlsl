#include "/Test/STF/ShaderTestFramework.hlsli"


[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void AllShouldPass()
{
    STF::AreEqual(1u, ttl::fundamental_type_traits<bool>::rank);
    STF::AreEqual(1u, ttl::fundamental_type_traits<int>::rank);
    STF::AreEqual(2u, ttl::fundamental_type_traits<int2>::rank);
    STF::AreEqual(3u, ttl::fundamental_type_traits<int3>::rank);
    STF::AreEqual(4u, ttl::fundamental_type_traits<int4>::rank);
    STF::AreEqual(1u, ttl::fundamental_type_traits<uint>::rank);
    STF::AreEqual(2u, ttl::fundamental_type_traits<uint2>::rank);
    STF::AreEqual(3u, ttl::fundamental_type_traits<uint3>::rank);
    STF::AreEqual(4u, ttl::fundamental_type_traits<uint4>::rank);
    STF::AreEqual(1u, ttl::fundamental_type_traits<float>::rank);
    STF::AreEqual(2u, ttl::fundamental_type_traits<float2>::rank);
    STF::AreEqual(3u, ttl::fundamental_type_traits<float3>::rank);
    STF::AreEqual(4u, ttl::fundamental_type_traits<float4>::rank);

    STF::IsTrue(ttl::is_same<bool, ttl::fundamental_type_traits<bool>::base_type>::value);
    STF::IsTrue(ttl::is_same<int, ttl::fundamental_type_traits<int>::base_type>::value);
    STF::IsTrue(ttl::is_same<int, ttl::fundamental_type_traits<int2>::base_type>::value);
    STF::IsTrue(ttl::is_same<int, ttl::fundamental_type_traits<int3>::base_type>::value);
    STF::IsTrue(ttl::is_same<int, ttl::fundamental_type_traits<int4>::base_type>::value);
    STF::IsTrue(ttl::is_same<uint, ttl::fundamental_type_traits<uint>::base_type>::value);
    STF::IsTrue(ttl::is_same<uint, ttl::fundamental_type_traits<uint2>::base_type>::value);
    STF::IsTrue(ttl::is_same<uint, ttl::fundamental_type_traits<uint3>::base_type>::value);
    STF::IsTrue(ttl::is_same<uint, ttl::fundamental_type_traits<uint4>::base_type>::value);
    STF::IsTrue(ttl::is_same<float, ttl::fundamental_type_traits<float>::base_type>::value);
    STF::IsTrue(ttl::is_same<float, ttl::fundamental_type_traits<float2>::base_type>::value);
    STF::IsTrue(ttl::is_same<float, ttl::fundamental_type_traits<float3>::base_type>::value);
    STF::IsTrue(ttl::is_same<float, ttl::fundamental_type_traits<float4>::base_type>::value);
}