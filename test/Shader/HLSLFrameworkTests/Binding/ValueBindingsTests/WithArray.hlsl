#include "/Test/STF/ShaderTestFramework.hlsli"

float Param[2];

[numthreads(1, 1, 1)]
void Main()
{
    ASSERT(AreEqual, Param[0], 2.0f);
    ASSERT(AreEqual, Param[1], 42.0f);
}