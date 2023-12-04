int BuffIndex;
int64_t InVal;
[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    RWBuffer<int64_t> Buff = ResourceDescriptorHeap[BuffIndex];
    Buff[DispatchThreadId.x] = (DispatchThreadId.x + 34) * InVal;
}