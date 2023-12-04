#define RS \
"RootFlags(" \
    "DENY_VERTEX_SHADER_ROOT_ACCESS |" \
    "DENY_HULL_SHADER_ROOT_ACCESS |" \
    "DENY_DOMAIN_SHADER_ROOT_ACCESS |" \
    "DENY_GEOMETRY_SHADER_ROOT_ACCESS |" \
    "DENY_PIXEL_SHADER_ROOT_ACCESS |" \
    "DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |" \
    "DENY_MESH_SHADER_ROOT_ACCESS |" \
    "CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED" \
")," \
"RootConstants(" \
    "num32BitConstants=2," \
    "b0" \

static bool StaticBool[
[RootSignature(RS)]
[numthreads(1,1,1)]
void GIVEN_WarpAndNoOptimizations_WHEN_ThisIsRun_THEN_ItWillSomeTimesFail(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    if(StaticBool[0])
    {
        RWByteAddressBuffer results = ResourceDescriptorHeap[0];
        uint old;
        results.InterlockedAdd(4, 1, old);
  
    for (int i = 0; i < 1; ++i)
    {
        static int value = 0;
        static int name = value;
        StaticBool[name] = true;
    }
}