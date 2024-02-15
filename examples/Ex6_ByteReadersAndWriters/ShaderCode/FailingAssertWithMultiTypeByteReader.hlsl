// "/Test" is the virtual shader directory for the framework
// This is defined by the framework itself.
#include "/Test/STF/ShaderTestFramework.hlsli"

template<uint InNum>
struct MyType
{
    uint Data[InNum];

    bool operator==(MyType Other)
    {
        return false;
    }
};

namespace STF
{
    
    // Since MyType has a non-type template parameter we partially specialize
    // ByteReaderTraits to get access to the non-type template parameter and use that
    // Parameter as our TypeId.
    // Using ByteReaderTraitsBase to fill out the members for us.
    template<uint InNum>
    struct ByteReaderTraits<MyType<InNum> > : ByteReaderTraitsBase<MY_TYPE_READER_ID, InNum>{};
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void Test()
{
    MyType<4> test1 = (MyType<4>)0;
    test1.Data[2] = 2;
    MyType<4> test2 = (MyType<4>)0;
    test2.Data[3] = 42;

    //ASSERT(AreEqual, test1, test2);
}