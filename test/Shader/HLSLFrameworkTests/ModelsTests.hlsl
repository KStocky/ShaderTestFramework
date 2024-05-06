
#include "/Test/TTL/models.hlsli"
#include "/Test/TTL/static_assert.hlsli"

struct ParamType1{};
struct ParamType2{};

struct ZeroParamFunc
{
    void operator()(){}
};

struct OneParamFunc
{
    void operator()(ParamType1){}
};

struct TwoParamFuncSingleType
{
    void operator()(ParamType1, ParamType1){}
};

struct TwoParamFuncTwoTypes
{
    void operator()(ParamType1, ParamType2){}
};

struct ZeroParamConcept
{
    template<typename T>
    __decltype(
        ttl::declval<T>()()
    ) requires();
};

struct OneParamConcept
{
    template<typename T, typename P>
    __decltype(
        ttl::declval<T>()(ttl::declval<P>())
    ) requires();
};

struct TwoParamSingleTypeConcept
{
    template<typename T, typename P>
    __decltype(
        ttl::declval<T>()(ttl::declval<P>(), ttl::declval<P>())
    ) requires();
};

struct TwoParamTwoTypesConcept
{
    template<typename T, typename P1, typename P2>
    __decltype(
        ttl::declval<T>()(ttl::declval<P1>(), ttl::declval<P2>())
    ) requires();
};

STATIC_ASSERT((ttl::models<ZeroParamConcept, ZeroParamFunc>::value));
STATIC_ASSERT((!ttl::models<ZeroParamConcept, OneParamFunc, ParamType1>::value));
STATIC_ASSERT((!ttl::models<ZeroParamConcept, TwoParamFuncSingleType, ParamType1>::value));
STATIC_ASSERT((!ttl::models<ZeroParamConcept, TwoParamFuncTwoTypes, ParamType1, ParamType2>::value));

STATIC_ASSERT((!ttl::models<OneParamConcept, ZeroParamFunc>::value));
STATIC_ASSERT((ttl::models<OneParamConcept, OneParamFunc, ParamType1>::value));
STATIC_ASSERT((!ttl::models<OneParamConcept, TwoParamFuncSingleType, ParamType1>::value));
STATIC_ASSERT((!ttl::models<OneParamConcept, TwoParamFuncTwoTypes, ParamType1, ParamType2>::value));

STATIC_ASSERT((!ttl::models<TwoParamSingleTypeConcept, ZeroParamFunc>::value));
STATIC_ASSERT((!ttl::models<TwoParamSingleTypeConcept, OneParamFunc, ParamType1>::value));
STATIC_ASSERT((ttl::models<TwoParamSingleTypeConcept, TwoParamFuncSingleType, ParamType1>::value));
STATIC_ASSERT((!ttl::models<TwoParamSingleTypeConcept, TwoParamFuncTwoTypes, ParamType1, ParamType2>::value));

STATIC_ASSERT((!ttl::models<TwoParamTwoTypesConcept, ZeroParamFunc>::value));
STATIC_ASSERT((!ttl::models<TwoParamTwoTypesConcept, OneParamFunc, ParamType1>::value));
STATIC_ASSERT((!ttl::models<TwoParamTwoTypesConcept, TwoParamFuncSingleType, ParamType1>::value));
STATIC_ASSERT((ttl::models<TwoParamTwoTypesConcept, TwoParamFuncTwoTypes, ParamType1, ParamType2>::value));