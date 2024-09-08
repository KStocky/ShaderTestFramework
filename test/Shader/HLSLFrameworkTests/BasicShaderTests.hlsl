#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void ThisTestShouldPass(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    stf::detail::Success();
}
                        
[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void ThisTestShouldFail(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    stf::Fail();
}