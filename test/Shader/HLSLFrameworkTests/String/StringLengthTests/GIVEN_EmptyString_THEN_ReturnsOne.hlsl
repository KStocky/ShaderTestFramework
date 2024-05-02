#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/TTL/string.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void Main()
{
    ASSERT(AreEqual, 1u, ttl::strlen(""));

    ttl::string test;
    ttl::zero(test);
    ASSERT(AreEqual, 0u, ttl::strlen(test));
}