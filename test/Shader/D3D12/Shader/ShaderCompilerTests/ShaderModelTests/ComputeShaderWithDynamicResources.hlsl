uint BuffIndex;
int InVal;
[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    RWBuffer<int> Buff = ResourceDescriptorHeap[BuffIndex];
    Buff[DispatchThreadId.x] = 34 * InVal;
}