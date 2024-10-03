// "/Test" is the virtual shader directory for the framework
// This is defined by the framework itself.
#include "/Test/STF/ShaderTestFramework.hlsli"

struct MyType
{
    uint a;
    float16_t b;
    bool c;

    bool operator==(MyType Other)
    {
        return false;
    }
};

[numthreads(1, 1, 1)]
void Test()
{
    MyType test1 = (MyType)0;
    test1.b = 2.0;
    MyType test2 = (MyType)0;
    test2.a = 42;

    //ASSERT(AreEqual, test1, test2);
}