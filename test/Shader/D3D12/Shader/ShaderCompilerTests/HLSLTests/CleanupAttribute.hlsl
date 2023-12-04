RWBuffer<int> MyBuffer;
void DoTheThing(){}

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    int x __attribute__((cleanup(DoTheThing)));
    MyBuffer[DispatchThreadId.x] = 4;
}