template<int ID>
struct Test
{
    static const int Num = 2;
};

RWBuffer<int> MyBuffer;

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
   MyBuffer[DispatchThreadId.x] = Test<42>::Num;
}