#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_IntArray_WHEN_Store3Called_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ttl::container_wrapper<int[3]> span;
    span.store(0, 42, 53, 78);

    ASSERT(AreEqual, 42, span.Data[0]);
    ASSERT(AreEqual, 53, span.Data[1]);
    ASSERT(AreEqual, 78, span.Data[2]);
}