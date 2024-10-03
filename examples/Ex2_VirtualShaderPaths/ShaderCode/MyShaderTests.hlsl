// We have set "/Shader" to be the virtual file directory for our shader code so we can
// use that to include headers
#include "/Shader/MyCoolHLSLFunction.hlsli"

// "/Test" is the virtual shader directory for the framework
// This is defined by the framework itself.
#include "/Test/STF/ShaderTestFramework.hlsli"

[numthreads(1, 1, 1)]
void Example2Test()
{
    ASSERT(AreEqual, ReturnTheBestNumber(), 42);
}