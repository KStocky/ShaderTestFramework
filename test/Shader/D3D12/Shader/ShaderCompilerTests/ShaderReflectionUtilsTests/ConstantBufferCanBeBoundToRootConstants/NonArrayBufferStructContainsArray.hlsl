
struct MyStruct
{
    int4 Data1[2];
};

ConstantBuffer<MyStruct> MyBuffer;

RWBuffer<int> Out;

[numthreads(1,1,1)]
void Main()
{
    Out[0] = MyBuffer.Data1[0].x;
}