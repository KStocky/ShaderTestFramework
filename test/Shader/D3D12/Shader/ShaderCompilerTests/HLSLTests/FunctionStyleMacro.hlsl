#define FUNC(InA, InB) InA + InB
RWBuffer<int> Buff;

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    Buff[DispatchThreadId.x] = FUNC(4, 3);
}