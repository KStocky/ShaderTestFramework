
struct MyStruct
{
    int4 Data;
};

ConstantBuffer<MyStruct> MyBuffer;

RWBuffer<int> Out;

[numthreads(1,1,1)]
void Main()
{
    Out[0] = MyBuffer.Data.x;
}