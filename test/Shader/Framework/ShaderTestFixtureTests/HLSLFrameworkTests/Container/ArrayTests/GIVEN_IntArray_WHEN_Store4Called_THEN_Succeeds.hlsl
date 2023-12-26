#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_IntArray_WHEN_Store4Called_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ttl::container<int[4]> span;
    span.store(0, 42, 55, 139, 1024);

    STF::AreEqual(42, span.Data[0]);
    STF::AreEqual(55, span.Data[1]);
    STF::AreEqual(139, span.Data[2]);
    STF::AreEqual(1024, span.Data[3]);
}