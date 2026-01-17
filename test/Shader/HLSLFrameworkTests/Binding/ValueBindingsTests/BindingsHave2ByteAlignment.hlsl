#include "/Test/STF/ShaderTestFramework.hlsli"

uint16_t Param1;
uint16_t Param2;
uint16_t Param3;
uint16_t Param4;

[numthreads(1, 1, 1)]
void Main()
{
    ASSERT(AreEqual, Param1, (uint16_t)2);
    ASSERT(AreEqual, Param2, (uint16_t)2);
    ASSERT(AreEqual, Param3, (uint16_t)2);
    ASSERT(AreEqual, Param4, (uint16_t)2);
}