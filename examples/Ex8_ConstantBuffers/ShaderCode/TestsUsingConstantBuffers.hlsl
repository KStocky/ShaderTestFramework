// "/Test" is the virtual shader directory for the framework
// This is defined by the framework itself.
#include "/Test/stf/ShaderTestFramework.hlsli"

struct MyStruct
{
    float3 Vec3;
    int2 IntPoint;
};

MyStruct GlobalBinding;

ConstantBuffer<MyStruct> CBuffer;

namespace MyNamespace
{
    int GlobalParam;
}

[numthreads(1, 1, 1)]
void TestWithConstantBuffers()
{
    ASSERT(AreEqual, GlobalBinding.IntPoint.x, 42);
    ASSERT(AreEqual, CBuffer.Vec3.z, 2.0);
    ASSERT(AreEqual, MyNamespace::GlobalParam, -1);
}