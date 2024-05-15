#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/TTL/string.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void Main()
{
    //65 characters
    DEFINE_STRING_CREATOR(creator, "01234567890123456789012345678901234567890123456789012345678901234");
}