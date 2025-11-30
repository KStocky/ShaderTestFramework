
#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/TTL/string.hlsli"

[numthreads(1,1,1)]
void Test()
{
    CREATE_STRING(actual, TEST_STRING);
    CREATE_STRING(expected, EXPECTED_STRING);

    ASSERT(AreEqual, actual.Size, expected.Size);
    for (uint i = 0; i < actual.Size; ++i)
    {
        ASSERT(AreEqual, actual.Data[i], expected.Data[i]);
    }
}
