#include "/Test/Public/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_IntArray_WHEN_Store2CalledWithDifferentType_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    STF::container<int[2]> span;
    span.store(0, 42u, 32);
}