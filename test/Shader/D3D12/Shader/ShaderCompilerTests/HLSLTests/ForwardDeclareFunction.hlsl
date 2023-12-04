int GetAnswer();
RWBuffer<int> MyBuffer;

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
   MyBuffer[DispatchThreadId.x] = GetAnswer();
}

int GetAnswer()
{
    return 42;
}