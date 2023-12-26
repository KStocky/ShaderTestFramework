#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_IntArray_WHEN_Store2Called_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ttl::container<int[2]> span;
    span.store(0, 42, 53);

    STF::AreEqual(42, span.Data[0]);
    STF::AreEqual(53, span.Data[1]);
}