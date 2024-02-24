#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/STF/StringHandling.hlsli"

using ShaderTestPrivate::StringLength;

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void Main()
{
    const uint val = StringLength("This is a super long string. Much longer than the library will support due to compiler limitations. This should fail to compile");
}