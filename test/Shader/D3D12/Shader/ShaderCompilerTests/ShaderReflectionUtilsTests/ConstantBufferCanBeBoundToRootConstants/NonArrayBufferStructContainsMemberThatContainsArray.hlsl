
struct MyOtherStruct
{
    int Data[2];
};

struct MyStruct
{
    MyOtherStruct Data;
};

ConstantBuffer<MyStruct> MyBuffer;

RWBuffer<int> Out;

[numthreads(1,1,1)]
void Main()
{
    Out[0] = MyBuffer.Data.Data[1].x;
}