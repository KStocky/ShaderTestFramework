
struct MyStruct
{
    uint A;
};

RWByteAddressBuffer myBuffer;

[numthreads(1,1,1)]
void Main()
{
    MyStruct test = myBuffer.Load<MyStruct>(0);
    test.A = 2;
    myBuffer.Store(0, test);
};