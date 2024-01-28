#pragma once

#define CONCAT_STATIC_ASSERT_IMPL(x, y) x##y
#define CONCAT_STATIC_ASSERT(x, y) CONCAT_STATIC_ASSERT_IMPL(x, y)
#define STATIC_ASSERT(Expression, ...) static const vector<int, !!(Expression)> CONCAT_STATIC_ASSERT(ASSERT_VAR_, __COUNTER__)