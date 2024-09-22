#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_IntArray_WHEN_LoadCalled_THEN_ReturnsExpectedValue(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ttl::container_wrapper<int[2]> span;
    span.Data[0] = 42;
    span.Data[1] = 53;
    const int actual = span.load(0);

    ASSERT(AreEqual, 42, actual);
}