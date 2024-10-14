
struct MyStruct
{
    int4 Data1;
    int4 Data2;
    int4 Data3;
    int4 Data4;
    int Data5;
};

ConstantBuffer<MyStruct> MyBuffer;

RWBuffer<int> Out;

[numthreads(1,1,1)]
void Main()
{
    Out[0] = MyBuffer.Data1.x;
}