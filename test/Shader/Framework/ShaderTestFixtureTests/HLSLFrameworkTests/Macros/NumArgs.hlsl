#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ZeroArgs_WHEN_Counted_THEN_ReturnsZero()
{
    STF::AreEqual(0, STF_NUM_ARGS());
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_OneArg_WHEN_Counted_THEN_ReturnsOne()
{
    STF::AreEqual(1, STF_NUM_ARGS(Hello));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TenArgs_WHEN_Counted_THEN_ReturnsTen()
{
    STF::AreEqual(10, STF_NUM_ARGS(a, b, c, d, e, f, g, h, i, j));
}