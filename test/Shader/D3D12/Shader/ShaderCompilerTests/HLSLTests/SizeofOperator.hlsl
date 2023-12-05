struct TestStruct
{
    int A;
    float B;
};

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    int a = sizeof(TestStruct);
}