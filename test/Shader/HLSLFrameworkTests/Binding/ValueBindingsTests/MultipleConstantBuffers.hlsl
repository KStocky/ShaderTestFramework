#include "/Test/STF/ShaderTestFramework.hlsli"

struct MyStruct
{
    float A;
    int B;
    float4 C;
};

ConstantBuffer<MyStruct> FirstParam;
ConstantBuffer<MyStruct> SecondParam;

[numthreads(1, 1, 1)]
void Main()
{
    ASSERT(AreEqual, FirstParam.A, 2.0f);
    ASSERT(AreEqual, FirstParam.B, 24);
    ASSERT(AreEqual, FirstParam.C, float4(5.0, 6.0, 7.0, 8.0));
    
    ASSERT(AreEqual, SecondParam.A, 102.5f);
    ASSERT(AreEqual, SecondParam.B, 4195);
    ASSERT(AreEqual, SecondParam.C, float4(5.0, 10.0, 15.0, 28.5));
}