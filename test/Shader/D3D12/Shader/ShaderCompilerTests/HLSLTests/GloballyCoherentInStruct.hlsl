
struct TestStruct
{
    globallycoherent RWBuffer<int> Data;
};

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
}  