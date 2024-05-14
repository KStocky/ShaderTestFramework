#ifndef TTL_MACRO_HEADER
#define TTL_MACRO_HEADER

#define TTL_JOIN(a, b) a##b

#define TTL_NUM_ARGS_N( \
          _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, \
         _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
         _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
         _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
         _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
         _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
         _61, _62, _63, N, ...) N

#define TTL_NUM_ARGS_RSEQ_N()                                        \
         62, 61, 60,                                       \
         59, 58, 57, 56, 55, 54, 53, 52, 51, 50,           \
         49, 48, 47, 46, 45, 44, 43, 42, 41, 40,           \
         39, 38, 37, 36, 35, 34, 33, 32, 31, 30,           \
         29, 28, 27, 26, 25, 24, 23, 22, 21, 20,           \
         19, 18, 17, 16, 15, 14, 13, 12, 11, 10,           \
          9,  8,  7,  6,  5,  4,  3,  2,  1,  0

#define TTL_NUM_ARGS_IMPL(...) TTL_NUM_ARGS_N(__VA_ARGS__)    

#define TTL_NUM_ARGS(...) TTL_NUM_ARGS_IMPL(Ignored, ##__VA_ARGS__, TTL_NUM_ARGS_RSEQ_N())

#define TTL_JOIN_MACRO(InStem, InSuffix, ...) TTL_JOIN(InStem, InSuffix)(__VA_ARGS__)

#define TTL_STAMP4(n, x) \
    x(n);                 \
    x(n + 1);             \
    x(n + 2);             \
    x(n + 3)
#define TTL_STAMP16(n, x) \
    TTL_STAMP4(n, x);     \
    TTL_STAMP4(n + 4, x); \
    TTL_STAMP4(n + 8, x); \
    TTL_STAMP4(n + 12, x)
#define TTL_STAMP64(n, x)   \
    TTL_STAMP16(n, x);      \
    TTL_STAMP16(n + 16, x); \
    TTL_STAMP16(n + 32, x); \
    TTL_STAMP16(n + 48, x)
#define TTL_STAMP256(n, x)   \
    TTL_STAMP64(n, x);       \
    TTL_STAMP64(n + 64, x);  \
    TTL_STAMP64(n + 128, x); \
    TTL_STAMP64(n + 192, x)

#define TTL_STAMP(n, x) x(TTL_STAMP##n, n)

#endif