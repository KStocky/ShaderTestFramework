
// '/Shader' is the virtual shader directory for the example
// shader code. It is defined in CompileTimeTests.cpp
#include "/Shader/ConditionalType.hlsli"

// "/Test" is the virtual shader directory for the framework
// This is defined by the framework itself.
// Since this is only a compile time test we only need to include the static assert header
// from the TTL. We do not need to include the full testing framework header.
#include "/Test/TTL/static_assert.hlsli"

// Without a message
STATIC_ASSERT((ttl::is_same<int, ConditionalType<true, int, float>::Type>::value));
STATIC_ASSERT((ttl::is_same<float, ConditionalType<false, int, float>::Type>::value));

// With a message
STATIC_ASSERT((ttl::is_same<int, ConditionalType<true, int, float>::Type>::value), "Expected the first type to be returned when the condition is true");
STATIC_ASSERT((ttl::is_same<float, ConditionalType<false, int, float>::Type>::value), "Expected the second type to be returned when the condition is false");
