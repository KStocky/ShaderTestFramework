
template<typename T>
struct Test
{
    static const uint value = 0;
};

template<typename T, uint Rows, uint Columns>
struct Test<row_major matrix<T, Rows, Columns> >
{
    static const uint value = 1;
};

template<typename T, uint Rows, uint Columns>
struct Test<column_major matrix<T, Columns, Rows> >
{
    static const uint value = 2;
};

template<bool Value>
struct StaticAssert;

template<>
struct StaticAssert<true>{};

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    StaticAssert<Test<float4x3>::value == 1> test;
};