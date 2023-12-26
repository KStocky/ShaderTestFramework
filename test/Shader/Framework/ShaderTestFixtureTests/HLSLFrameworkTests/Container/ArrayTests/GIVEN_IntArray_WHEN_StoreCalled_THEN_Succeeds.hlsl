#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_IntArray_WHEN_StoreCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    STF::container<int[2]> span;
    span.store(0, 42);

    STF::AreEqual(42, span.Data[0]);
}