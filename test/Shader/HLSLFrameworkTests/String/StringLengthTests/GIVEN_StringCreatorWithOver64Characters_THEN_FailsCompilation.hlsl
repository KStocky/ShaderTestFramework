#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/TTL/string.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void Main()
{
    DEFINE_STRING_CREATOR(creator, "This is a super long string. Much longer than the library will support due to compiler limitations. This should fail to compile");
}