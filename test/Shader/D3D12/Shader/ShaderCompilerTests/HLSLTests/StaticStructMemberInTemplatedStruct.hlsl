template<int ID>
struct Test
{
    static int Num;
};

template<int ID>
int Test<ID>::Num = 2;

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
   int i = Test<0>::Num;
}