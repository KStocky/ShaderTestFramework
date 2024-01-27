#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/TTL/static_assert.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void Main()
{
    STATIC_ASSERT(42 != 42 && 1 < 3);
}