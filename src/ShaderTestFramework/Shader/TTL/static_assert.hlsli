#ifndef TTL_STATIC_ASSERT_HEADER
#define TTL_STATIC_ASSERT_HEADER

#include "/Test/TTL/macro.hlsli"
#include "/Test/TTL/type_traits.hlsli"

#define STATIC_ASSERT_IF_0(InExpression) _Static_assert(InExpression, "")
#define STATIC_ASSERT_IF_1(InExpression, InMessage) _Static_assert(InExpression, InMessage)
#define STATIC_ASSERT(InExpression, ...) TTL_JOIN_MACRO(STATIC_ASSERT_IF_, TTL_NUM_ARGS(__VA_ARGS__), InExpression, ##__VA_ARGS__)



#endif