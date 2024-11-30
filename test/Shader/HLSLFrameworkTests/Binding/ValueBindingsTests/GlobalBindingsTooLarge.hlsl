#include "/Test/STF/ShaderTestFramework.hlsli"

struct MyStruct
{
    float A;
    int B;
    float4 C;
};

MyStruct MyParam;

float2 D;
int3 E;

int4x4 F;
int4x4 G;
int4x4 H;

[numthreads(1, 1, 1)]
void Main()
{
    ASSERT(AreEqual, MyParam.A, 4.0f);
    ASSERT(AreEqual, MyParam.B, 42);
    ASSERT(AreEqual, MyParam.C, float4(1.0, 2.0, 3.0, 4.0));
    ASSERT(AreEqual, D, float2(5.0, 6.0));
    ASSERT(AreEqual, E, int3(123, 456, 789));
}