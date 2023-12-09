
template<typename T>
struct A
{
    template<typename U>
    struct B
    {
        U value;
    };

    B<T> value;
};

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    A<int> test;
}