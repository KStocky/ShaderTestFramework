struct MyStruct
{
    int operator ()()
    {
        return 0;
    }
};

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    MyStruct m;
    int i = m();
}