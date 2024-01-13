// We have set "/Shader" to be the virtual file directory for our shader code so we can
// use that to include headers
#include "/Shader/MyArray.hlsli"

// "/Test" is the virtual shader directory for the framework
// This is defined by the framework itself.
#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void Example3Tests()
{
    STF::AreEqual(42, 42);
}