#include "/Test/STF/ShaderTestFramework.hlsli"

float Param[2];

[numthreads(1, 1, 1)]
void Main()
{
    ASSERT(AreEqual, Param[0], 4.0f);
}