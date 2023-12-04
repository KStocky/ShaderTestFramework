RWTexture2DMS<float4, 4> Buff;

float4 InVal;
[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    Buff[DispatchThreadId.xy] = InVal;
}