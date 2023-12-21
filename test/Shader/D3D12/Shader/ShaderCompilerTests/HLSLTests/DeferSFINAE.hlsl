template<typename T, T v>
struct integral_constant
{
    static const T value = v;
    using value_type = T;
    using type = integral_constant;
};

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

template<typename T, typename U>
struct is_same : false_type
{
};

template<typename T>
struct is_same<T, T> : true_type 
{
};

template<bool InCond, typename T = void>
struct enable_if
{
};

template<typename T>
struct enable_if<true, T>
{
    using type = T;  
};

struct Test
{
    template<typename T>
    static typename enable_if<is_same<T, uint>::value, int>::type Foo()
    {
        return 42;
    }

    template<typename T>
    static typename enable_if<!is_same<T, uint>::value, int>::type Foo()
    {
        return Foo<uint>();
    }
};

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const int ans = Test::Foo<float>();
}