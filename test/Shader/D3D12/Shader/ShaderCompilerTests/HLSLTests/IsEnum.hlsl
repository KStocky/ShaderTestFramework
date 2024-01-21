enum class MyEnum
{
    One,
    Two
};

[numthreads(1,1,1)]
void Main()
{
    bool result = __is_enum(MyEnum);
}