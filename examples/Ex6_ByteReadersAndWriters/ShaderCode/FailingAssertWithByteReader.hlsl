// "/Test" is the virtual shader directory for the framework
// This is defined by the framework itself.
#include "/Test/STF/ShaderTestFramework.hlsli"

struct MyType
{
    uint a;
    float64_t b;
    bool c;

    bool operator==(MyType Other)
    {
        return false;
    }
};

namespace STF
{
    // Using ByteReaderTraitsBase to fill out the members for us.
    template<>
    struct ByteReaderTraits<MyType> : ByteReaderTraitsBase<MY_TYPE_READER_ID>{};
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void Test()
{
    MyType test1 = (MyType)0;
    test1.b = 2.0;
    MyType test2 = (MyType)0;
    test2.a = 42;

    ASSERT(AreEqual, test1, test2);
}