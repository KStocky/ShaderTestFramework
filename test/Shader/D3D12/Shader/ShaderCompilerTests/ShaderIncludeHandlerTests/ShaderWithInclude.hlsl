#include "/Tests/ShaderIncludeHandlerTests/Include.hlsli"

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    hello();
}