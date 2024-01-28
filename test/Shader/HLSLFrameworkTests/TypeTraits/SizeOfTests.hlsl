#include "/Test/STF/ShaderTestFramework.hlsli"

enum A
{
    One
};

enum class B
{
    One
};

struct C
{
    int D;
};

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void AllSucceed()
{
    ASSERT(AreEqual, 4u, ttl::size_of<A>::value);
    ASSERT(AreEqual, 4u, ttl::size_of<B>::value);
    ASSERT(AreEqual, 4u, ttl::size_of<C>::value);
    ASSERT(AreEqual, 4u, ttl::size_of<uint>::value);
    ASSERT(AreEqual, 8u, ttl::size_of<uint64_t>::value);
    ASSERT(AreEqual, 2u, ttl::size_of<uint16_t>::value);
    ASSERT(AreEqual, 24u, ttl::size_of<uint64_t3>::value);
    ASSERT(AreEqual, 8u, ttl::size_of<uint16_t4>::value);
}