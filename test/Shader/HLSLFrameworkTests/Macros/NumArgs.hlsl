#include "/Test/TTL/macro.hlsli"

_Static_assert(0 == TTL_NUM_ARGS(), "Expected NUM_ARGS to return 0");
_Static_assert(1 == TTL_NUM_ARGS(Hello), "Expected NUM_ARGS to return 1");
_Static_assert(10 == TTL_NUM_ARGS(a, b, c, d, e, f, g, h, i, j), "Expected NUM_ARGS to return 10");