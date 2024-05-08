
#include "/Test/TTL/static_assert.hlsli"
#include "/Test/TTL/type_traits.hlsli"

struct ParamType{};
struct NotParamType{};

void ZeroParamFunction(){}
void OneParamFunction(ParamType){}
void TwoParamFunction(ParamType, ParamType){}
void ThreeParamFunction(ParamType, ParamType, ParamType){}
void FourParamFunction(ParamType, ParamType, ParamType, ParamType){}
void FiveParamFunction(ParamType, ParamType, ParamType, ParamType, ParamType){}

STATIC_ASSERT((ttl::is_invocable_function<__decltype(ZeroParamFunction)>::value));
STATIC_ASSERT((ttl::is_invocable_function<__decltype(OneParamFunction), ParamType>::value));
STATIC_ASSERT((ttl::is_invocable_function<__decltype(TwoParamFunction), ParamType, ParamType>::value));
STATIC_ASSERT((ttl::is_invocable_function<__decltype(ThreeParamFunction), ParamType, ParamType, ParamType>::value));
STATIC_ASSERT((ttl::is_invocable_function<__decltype(FourParamFunction), ParamType, ParamType, ParamType, ParamType>::value));
STATIC_ASSERT((ttl::is_invocable_function<__decltype(FiveParamFunction), ParamType, ParamType, ParamType, ParamType, ParamType>::value));

STATIC_ASSERT((!ttl::is_invocable_function<__decltype(OneParamFunction)>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(TwoParamFunction)>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(ThreeParamFunction)>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(FourParamFunction)>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(FiveParamFunction)>::value));

STATIC_ASSERT((!ttl::is_invocable_function<__decltype(ZeroParamFunction), NotParamType>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(OneParamFunction), NotParamType>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(TwoParamFunction), NotParamType, ParamType>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(ThreeParamFunction), NotParamType, ParamType, ParamType>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(FourParamFunction), NotParamType, ParamType, ParamType, ParamType>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(FiveParamFunction), NotParamType, ParamType, ParamType, ParamType, ParamType>::value));

STATIC_ASSERT((!ttl::is_invocable_function<__decltype(TwoParamFunction), ParamType, NotParamType>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(ThreeParamFunction), ParamType, NotParamType, ParamType>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(FourParamFunction), ParamType, NotParamType, ParamType, ParamType>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(FiveParamFunction), ParamType, NotParamType, ParamType, ParamType, ParamType>::value));

STATIC_ASSERT((!ttl::is_invocable_function<__decltype(ThreeParamFunction), ParamType, ParamType, NotParamType>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(FourParamFunction), ParamType, ParamType, NotParamType, ParamType>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(FiveParamFunction), ParamType, ParamType, NotParamType, ParamType, ParamType>::value));

STATIC_ASSERT((!ttl::is_invocable_function<__decltype(FourParamFunction), ParamType, ParamType, ParamType, NotParamType>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(FiveParamFunction), ParamType, ParamType, ParamType, NotParamType, ParamType>::value));

STATIC_ASSERT((!ttl::is_invocable_function<__decltype(FiveParamFunction), ParamType, ParamType, ParamType, ParamType, NotParamType>::value));

STATIC_ASSERT((!ttl::is_invocable_function<__decltype(OneParamFunction), NotParamType>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(TwoParamFunction), NotParamType, NotParamType>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(ThreeParamFunction), NotParamType, NotParamType, NotParamType>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(FourParamFunction), NotParamType, NotParamType, NotParamType, NotParamType>::value));
STATIC_ASSERT((!ttl::is_invocable_function<__decltype(FiveParamFunction), NotParamType, NotParamType, NotParamType, NotParamType, NotParamType>::value));