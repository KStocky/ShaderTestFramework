#include "/Test/Public/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_IntArray_WHEN_Store3Called_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    STF::container<int[3]> span;
    span.store(0, 42, 53, 78);

    STF::AreEqual(42, span.Data[0]);
    STF::AreEqual(53, span.Data[1]);
    STF::AreEqual(78, span.Data[2]);
}