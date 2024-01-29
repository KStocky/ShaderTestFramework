
// '/Shader' is the virtual shader directory for the example
// shader code. It is defined in CompileTimeTests.cpp
#include "/Shader/ConditionalType.hlsli"

// "/Test" is the virtual shader directory for the framework
// This is defined by the framework itself.
// Since this is only a compile time test we only need to include the static assert header
// from the TTL. We do not need to include the full testing framework header.
#include "/Test/TTL/static_assert.hlsli"


// We do not need to specify a root signature here since we will never actually run this
// shader. We will just see if it compiles or not.
[numthreads(1, 1, 1)]
void AllCompile()
{
}
