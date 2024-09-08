#include "/Test/STF/ShaderTestFramework.hlsli"

#define PP_ARG_N( \
          _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, \
         _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
         _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
         _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
         _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
         _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
         _61, _62, _63, N, ...) N

#define PP_RSEQ_N()                                        \
         62, 61, 60,                                       \
         59, 58, 57, 56, 55, 54, 53, 52, 51, 50,           \
         49, 48, 47, 46, 45, 44, 43, 42, 41, 40,           \
         39, 38, 37, 36, 35, 34, 33, 32, 31, 30,           \
         29, 28, 27, 26, 25, 24, 23, 22, 21, 20,           \
         19, 18, 17, 16, 15, 14, 13, 12, 11, 10,           \
          9,  8,  7,  6,  5,  4,  3,  2,  1,  0

#define PP_NARG_(...)    PP_ARG_N(__VA_ARGS__)    

#define PP_NARG(...)     PP_NARG_(Ignored, ##__VA_ARGS__, PP_RSEQ_N())

#define COUNT_ARG_TEST(...) PP_NARG(__VA_ARGS__)

#define IF_ARGS_0 Test = 0
#define IF_ARGS_1 Test = 1
#define IF_ARGS_2 Test = 2

#define JOIN(a, b) a##b

#define IF_ARGS_IMPL(a, b) JOIN(a, b)
#define IF_ARGS(...) IF_ARGS_IMPL(IF_ARGS_, PP_NARG(__VA_ARGS__))

#define VARIADIC_OVERLOAD(...) IF_ARGS(__VA_ARGS__)

#define IF_WITH_PARAMS_0()
#define IF_WITH_PARAMS_1(a) Test[0] = a
#define IF_WITH_PARAMS_2(a, b) Test[0] = a; Test[1] = b

#define IF_WITH_PARAMS_IMPL(a, b, ...) JOIN(a,b)(__VA_ARGS__)
#define IF_WITH_PARAMS(...) IF_WITH_PARAMS_IMPL(IF_WITH_PARAMS_, PP_NARG(__VA_ARGS__), __VA_ARGS__)
#define VARIADIC_OVERLOAD_PARAMS(...) IF_WITH_PARAMS(__VA_ARGS__)

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void VariadicMacroOverloading(uint3 DispatchThreadId : SV_DispatchThreadID)
{
	ASSERT(AreEqual, 0, COUNT_ARG_TEST());
    ASSERT(AreEqual, 1, COUNT_ARG_TEST(Hello World));
    ASSERT(AreEqual, 3, COUNT_ARG_TEST(Shader, Test, Framework));
    
    {
        int Test = -1;
        
        VARIADIC_OVERLOAD();
        ASSERT(AreEqual, 0, Test);

        VARIADIC_OVERLOAD(Hello);
        ASSERT(AreEqual, 1, Test);

        VARIADIC_OVERLOAD(Hello, World);
        ASSERT(AreEqual, 2, Test);
    }

    {
        int Test[2] = {0, 0};
        
        VARIADIC_OVERLOAD_PARAMS();
        ASSERT(AreEqual, 0, Test[0]);
        ASSERT(AreEqual, 0, Test[1]);

        VARIADIC_OVERLOAD_PARAMS(42);
        ASSERT(AreEqual, 42, Test[0]);
        ASSERT(AreEqual, 0, Test[1]);

        VARIADIC_OVERLOAD_PARAMS(42, 23);
        ASSERT(AreEqual, 42, Test[0]);
        ASSERT(AreEqual, 23, Test[1]);
    }
}