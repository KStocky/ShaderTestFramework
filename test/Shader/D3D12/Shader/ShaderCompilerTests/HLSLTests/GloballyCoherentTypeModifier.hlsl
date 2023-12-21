
template<typename T>
struct Test;

template<>
struct Test<RWBuffer<int> >
{
    int a;
};

template<>
struct Test<globallycoherent RWBuffer<int> >
{
    int b;
};

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
} 