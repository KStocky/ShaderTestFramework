template<int ID>
int StaticFunc(int In, bool InShouldChange)
{
    static int var = 0;
    if (InShouldChange)
    {
        var = In;
    }
    return var;
}

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
   int i = StaticFunc<0>(5, true);
}