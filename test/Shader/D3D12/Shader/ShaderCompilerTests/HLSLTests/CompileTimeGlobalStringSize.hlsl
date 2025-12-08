
template<typename T, T v>
struct integral_constant
{
    static const T value = v;
    using value_type = T;
    using type = integral_constant;
};

template<typename T, uint N>
integral_constant<uint, N> array_len(T In[N]);

using StringLengthType = __decltype(array_len("Hello"));
static const uint StringLength = StringLengthType::value;

[numthreads(1,1,1)]
void Main()
{
}