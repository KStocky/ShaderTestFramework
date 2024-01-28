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
    ASSERT(IsTrue, ttl::is_enum<A>::value);
    ASSERT(IsTrue, ttl::is_enum<B>::value);
    ASSERT(IsFalse, ttl::is_enum<C>::value);
    ASSERT(IsFalse, ttl::is_enum<uint>::value);
}