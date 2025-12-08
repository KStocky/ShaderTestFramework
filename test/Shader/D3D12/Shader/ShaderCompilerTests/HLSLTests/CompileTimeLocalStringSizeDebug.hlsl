
template<typename T, T v>
struct integral_constant
{
    static const T value = v;
    using value_type = T;
    using type = integral_constant;
};

template<typename T, uint N>
integral_constant<uint, N> array_len(T In[N]);

[numthreads(1,1,1)]
void Main()
{
    using StringLengthType = __decltype(array_len("Hello"));
    const uint StringLength = StringLengthType::value;
}