struct MyStruct
{
    int hi;
    operator bool()
    {
        return true;
    }
};

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    MyStruct testStruct;
    bool t = (bool)testStruct;
}