RWBuffer<int> MyBuffer;

struct TestStruct
{
    int GetAnswer();
};

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct t;
    MyBuffer[DispatchThreadId.x] = t.GetAnswer();

    int TestStruct::GetAnswer()
    {
        return 42;
    }
}