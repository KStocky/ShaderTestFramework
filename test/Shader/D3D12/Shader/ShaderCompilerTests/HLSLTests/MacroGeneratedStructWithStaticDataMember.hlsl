struct Test
{
    static int Num;
};

int Test::Num = 2;
RWBuffer<int> MyBuffer;

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
   MyBuffer[DispatchThreadId.x] = Test::Num;
}