
enum class MyEnum
{
    One,
    Two
};

[numthreads(1,1,1)]
void Main()
{
    uint size = sizeof(MyEnum);
}