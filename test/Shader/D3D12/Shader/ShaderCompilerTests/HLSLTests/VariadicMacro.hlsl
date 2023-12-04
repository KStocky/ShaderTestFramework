#define ENTRY(InName, ...) void InName(__VA_ARGS__)

[numthreads(1,1,1)]
ENTRY(Main, uint3 DispatchThreadId : SV_DispatchThreadID, uint GroupIndex : SV_GroupIndex)
{
}