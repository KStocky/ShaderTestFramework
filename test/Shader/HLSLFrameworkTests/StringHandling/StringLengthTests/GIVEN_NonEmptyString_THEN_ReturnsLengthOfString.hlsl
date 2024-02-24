#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/STF/StringHandling.hlsli"

using ShaderTestPrivate::StringLength;

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void Main()
{
    ASSERT(AreEqual, 13u, StringLength("Hello there!"));
}