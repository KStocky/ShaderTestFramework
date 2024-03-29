#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/TTL/byte_writer.hlsli"
#include "/Test/TTL/memory.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_Uninitialized_WHEN_BytesRequiredQueried_THEN_ZeroReturned()
{
    ShaderTestPrivate::PerThreadScratchData data;
    ttl::zero(data);
    ASSERT(AreEqual, ttl::bytes_required(data), 0u);
}

