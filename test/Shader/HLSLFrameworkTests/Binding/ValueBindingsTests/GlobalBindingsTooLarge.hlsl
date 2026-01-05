#include "/Test/STF/ShaderTestFramework.hlsli"

struct MyStruct
{
    float A;
    int B;
    float4 C;
};

MyStruct Param1;

struct TooLarge
{
    float2 D;
    int3 E;

    int4x4 F;
    int4x4 G;
    int4x4 H;
};

ConstantBuffer<TooLarge> Param2;

[numthreads(1, 1, 1)]
void Main()
{
    ASSERT(AreEqual, Param1.A, 4.0f);
    ASSERT(AreEqual, Param2.E.x, 42);
}