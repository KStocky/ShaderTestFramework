// "/Test" is the virtual shader directory for the framework
// This is defined by the framework itself.
#include "/Test/STF/ShaderTestFramework.hlsli"

const static int MyInts[] = {2, 4, 5, 6, 7, 42, 512, 1024, 3};

struct MyIntsSpan
{
    uint Start;
    uint End;

    static MyIntsSpan Create(uint InStart, uint InEnd)
    {
        MyIntsSpan ret;
        ret.Start = InStart;
        ret.End = InEnd;
        return ret;
    }

    bool operator==(MyIntsSpan)
    {
        return false;
    }
};

namespace stf
{
    // Using ByteReaderTraitsBase to fill out the members for us.
    template<>
    struct ByteReaderTraits<MyIntsSpan> : ByteReaderTraitsBase<MY_TYPE_READER_ID>{};
}

namespace ttl
{
    // Specialization of ttl::byte_writer for MyIntsSpan
    template<>
    struct byte_writer<MyIntsSpan>
    {
        static const bool has_writer = true;

        static uint bytes_required(MyIntsSpan In)
        {
            return (In.End - In.Start) * ttl::size_of<int>::value;
        }

        static uint alignment_required(MyIntsSpan In)
        {
            return ttl::align_of<int>::value;
        }

        // inout here is very important to make sure that the Container is returned out of the function
        template<typename U>
        static void write(inout container_wrapper<U> InContainer, const uint InIndex, const MyIntsSpan In)
        {
            const uint size = In.End - In.Start;
            for (uint i = 0; i < size; ++i)
            {
                InContainer.store(InIndex + i * 4u, MyInts[In.Start + i]);
            }
        }
    };
}

[numthreads(1, 1, 1)]
void Test()
{
    MyIntsSpan test1 = MyIntsSpan::Create(1, 4);
    MyIntsSpan test2 = MyIntsSpan::Create(2, 7);

    //ASSERT(AreEqual, test1, test2);
}