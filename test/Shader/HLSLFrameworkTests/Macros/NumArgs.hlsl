#include "/Test/TTL/macro.hlsli"
#include "/Test/TTL/static_assert.hlsli"

STATIC_ASSERT(1 == TTL_NUM_ARGS(), "Expected NUM_ARGS to return 0");
STATIC_ASSERT(1 == TTL_NUM_ARGS(Hello), "Expected NUM_ARGS to return 1");
STATIC_ASSERT(10 == TTL_NUM_ARGS(a, b, c, d, e, f, g, h, i, j), "Expected NUM_ARGS to return 10");