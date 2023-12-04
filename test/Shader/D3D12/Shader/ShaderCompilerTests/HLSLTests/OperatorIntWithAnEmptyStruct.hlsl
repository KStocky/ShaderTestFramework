struct MyStruct
{
    operator int()
    {
        return 0;
    }
};

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    MyStruct testStruct;
    int u = (int)testStruct;
}