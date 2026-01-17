#include "/Test/STF/ShaderTestFramework.hlsli"

struct MyStruct
{
    float A;
};

ConstantBuffer<MyStruct> Buffs[2];

[numthreads(1, 1, 1)]
void Main()
{
    ASSERT(AreEqual, Buffs[0].A, 4.0f);
}