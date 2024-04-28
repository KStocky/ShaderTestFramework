#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/TTL/string.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void Main()
{
    ASSERT(AreEqual, 13u, ttl::strlen("Hello there!"));
    DEFINE_STRING_CREATOR(testCreator, "Hello there!");
    ttl::string test = testCreator();

    ASSERT(AreEqual, 13u, ttl::strlen("Hello there!"));
}