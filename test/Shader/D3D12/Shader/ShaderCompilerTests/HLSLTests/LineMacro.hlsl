[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    int a = __LINE__;
    int b = __LINE__;
}