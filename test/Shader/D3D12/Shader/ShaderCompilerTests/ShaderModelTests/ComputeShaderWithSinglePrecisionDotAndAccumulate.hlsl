RWBuffer<uint> Buff;

uint InVal;
uint InVal2;
uint InVal3;

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    Buff[DispatchThreadId.x] = dot4add_u8packed(InVal, InVal2, InVal3);
}