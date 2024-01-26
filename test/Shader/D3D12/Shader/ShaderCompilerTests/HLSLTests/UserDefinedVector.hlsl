
struct MyStruct
{
    uint A;
};

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    vector<MyStruct, 4> test;
};