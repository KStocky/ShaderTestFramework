template<typename ContainerType>
struct span;

template<typename T, uint Size>
struct span<T[Size]>
{
    
};

template<>
struct span<RWByteAddressBuffer>
{
    
};

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    span<int[6]> a;
    span<RWByteAddressBuffer> b;
}