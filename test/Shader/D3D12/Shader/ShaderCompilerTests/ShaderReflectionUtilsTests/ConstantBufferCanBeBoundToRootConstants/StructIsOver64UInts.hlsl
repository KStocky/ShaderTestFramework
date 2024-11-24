
struct MyStruct
{
    int4x4 Data1;
    int4x4 Data2;
    int4x4 Data3;
    int4x4 Data4;
    int Data5;
};

ConstantBuffer<MyStruct> MyBuffer;

RWBuffer<int> Out;

[numthreads(1,1,1)]
void Main()
{
    Out[0] = MyBuffer.Data1[0][0];
}