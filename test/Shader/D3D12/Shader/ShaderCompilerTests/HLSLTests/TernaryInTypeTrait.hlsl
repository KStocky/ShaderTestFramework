
template<typename T, typename U>
struct Test
{
    static const bool a = sizeof(U) == 8;
    static const bool b = sizeof(U) == 4;

    static const bool value = sizeof(T) > 4 ? a : b;
};

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
   const bool i = Test<uint3, uint>::value;
}