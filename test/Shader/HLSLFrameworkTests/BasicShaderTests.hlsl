#include "/Test/STF/ShaderTestFramework.hlsli"

[numthreads(1, 1, 1)]
void ThisTestShouldPass(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    stf::AssertionsV1::detail::Success();
}
                        
[numthreads(1, 1, 1)]
void ThisTestShouldFail(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    stf::AssertionsV1::Fail();
}