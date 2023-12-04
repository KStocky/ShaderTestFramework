RWBuffer<int> Buff[2];

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    Buff[0][DispatchThreadId.x] = 0;
    Buff[1][DispatchThreadId.x] = 0;
}