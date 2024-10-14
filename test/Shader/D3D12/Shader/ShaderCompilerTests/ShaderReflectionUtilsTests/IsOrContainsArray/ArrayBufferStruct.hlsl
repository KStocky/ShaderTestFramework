
struct MyStruct
{
    int4 Data1;
};

ConstantBuffer<MyStruct> MyBuffer[2];

RWBuffer<int> Out;

[numthreads(1,1,1)]
void Main()
{
    Out[0] = MyBuffer[0].Data1.x;
}