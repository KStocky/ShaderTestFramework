#include "/MyTests/ShaderHeader.hlsli"

#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void ThisTestShouldPass(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(AreEqual, Identity(42), 42);
}
                        
[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void ThisTestShouldFail(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(Fail);
}
