
#include "/Test/TTL/static_assert.hlsli"
#include "/Test/TTL/type_traits.hlsli"

struct ParamType{};
struct NotParamType{};

struct Functor
{
    void operator()(){}
};

void ZeroParamFunction(){}
void OneParamFunction(ParamType){}
void TwoParamFunction(ParamType, ParamType){}
void ThreeParamFunction(ParamType, ParamType, ParamType){}
void FourParamFunction(ParamType, ParamType, ParamType, ParamType){}
void FiveParamFunction(ParamType, ParamType, ParamType, ParamType, ParamType){}

STATIC_ASSERT((ttl::is_function<__decltype(ZeroParamFunction)>::value));
STATIC_ASSERT((ttl::is_function<__decltype(OneParamFunction)>::value));
STATIC_ASSERT((ttl::is_function<__decltype(TwoParamFunction)>::value));
STATIC_ASSERT((ttl::is_function<__decltype(ThreeParamFunction)>::value));
STATIC_ASSERT((ttl::is_function<__decltype(FourParamFunction)>::value));
STATIC_ASSERT((ttl::is_function<__decltype(FiveParamFunction)>::value));

STATIC_ASSERT((!ttl::is_function<Functor>::value));
STATIC_ASSERT((!ttl::is_function<ParamType>::value));
