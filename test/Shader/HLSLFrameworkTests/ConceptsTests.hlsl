
#include "/Test/TTL/concepts.hlsli"
#include "/Test/TTL/static_assert.hlsli"

namespace EqualityComparableTests
{
    struct A
    {

    };

    struct B
    {
        bool operator==(A In)
        {
            return true;
        }

        bool operator!=(A In)
        {
            return false;
        }
    };

    struct C
    {
        int operator==(A In)
        {
            return 1;
        }

        bool operator!=(A In)
        {
            return false;
        }
    };

    struct D
    {
        bool operator==(A In)
        {
            return true;
        }

        int operator!=(A In)
        {
            return 1;
        }
    };

    struct E
    {
        int operator==(A In)
        {
            return 1;
        }

        int operator!=(A In)
        {
            return 1;
        }
    };

    STATIC_ASSERT((!ttl::models<ttl::equality_comparable, A, A>::value));
    STATIC_ASSERT((!ttl::models<ttl::equality_comparable, A, B>::value));
    STATIC_ASSERT((ttl::models<ttl::equality_comparable, B, A>::value));
    STATIC_ASSERT((!ttl::models<ttl::equality_comparable, C, A>::value));
    STATIC_ASSERT((!ttl::models<ttl::equality_comparable, D, A>::value));
    STATIC_ASSERT((!ttl::models<ttl::equality_comparable, E, A>::value));
}

namespace StringLiteralTests
{
    struct A
    {
    };

    STATIC_ASSERT((!ttl::models<ttl::string_literal, A[6]>::value));
    STATIC_ASSERT((!ttl::models<ttl::string_literal, A>::value));

    STATIC_ASSERT((ttl::models<ttl::string_literal, __decltype("Hello")>::value));
}

namespace InvocableFunctorTests
{
    struct NotParamType{};
    struct ParamType{};
    struct NotInvocableFunctor{};

    struct ZeroParamFunctor
    {
        void operator()(){}
    };

    struct OneParamFunctor
    {
        void operator()(ParamType){}
    };

    struct TwoParamFunctor
    {
        void operator()(ParamType, ParamType){}
    };

    struct ThreeParamFunctor
    {
        void operator()(ParamType, ParamType, ParamType){}
    };

    struct FourParamFunctor
    {
        void operator()(ParamType, ParamType, ParamType, ParamType){}
    };

    struct FiveParamFunctor
    {
        void operator()(ParamType, ParamType, ParamType, ParamType, ParamType){}
    };

    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, NotInvocableFunctor>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, NotInvocableFunctor, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, NotInvocableFunctor, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, NotInvocableFunctor, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, NotInvocableFunctor, ParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, NotInvocableFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value));

    STATIC_ASSERT(( ttl::models<ttl::invocable_functor, ZeroParamFunctor>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, ZeroParamFunctor, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, ZeroParamFunctor, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, ZeroParamFunctor, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, ZeroParamFunctor, ParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, ZeroParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value));

    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, OneParamFunctor>::value));
    STATIC_ASSERT((ttl::models<ttl::invocable_functor, OneParamFunctor, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, OneParamFunctor, NotParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, OneParamFunctor, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, OneParamFunctor, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, OneParamFunctor, ParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, OneParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value));

    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, TwoParamFunctor>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, TwoParamFunctor, ParamType>::value));
    STATIC_ASSERT((ttl::models<ttl::invocable_functor, TwoParamFunctor, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, TwoParamFunctor, NotParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, TwoParamFunctor, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, TwoParamFunctor, ParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, TwoParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value));

    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, ThreeParamFunctor>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, ThreeParamFunctor, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, ThreeParamFunctor, ParamType, ParamType>::value));
    STATIC_ASSERT((ttl::models<ttl::invocable_functor, ThreeParamFunctor, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, ThreeParamFunctor, NotParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, ThreeParamFunctor, ParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, ThreeParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value));

    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, FourParamFunctor>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, FourParamFunctor, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, FourParamFunctor, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, FourParamFunctor, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((ttl::models<ttl::invocable_functor, FourParamFunctor, ParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, FourParamFunctor, NotParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, FourParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value));

    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, FiveParamFunctor>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, FiveParamFunctor, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, FiveParamFunctor, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, FiveParamFunctor, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, FiveParamFunctor, ParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((ttl::models<ttl::invocable_functor, FiveParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable_functor, FiveParamFunctor, NotParamType, ParamType, ParamType, ParamType, ParamType>::value));
}

namespace InvocableTests
{
    struct NotParamType{};
    struct ParamType{};
    struct NotInvocableFunctor{};

    void ZeroParamFunction(){}
    void OneParamFunction(ParamType){}
    void TwoParamFunction(ParamType, ParamType){}
    void ThreeParamFunction(ParamType, ParamType, ParamType){}
    void FourParamFunction(ParamType, ParamType, ParamType, ParamType){}
    void FiveParamFunction(ParamType, ParamType, ParamType, ParamType, ParamType){}

    struct ZeroParamFunctor
    {
        void operator()(){}
    };

    struct OneParamFunctor
    {
        void operator()(ParamType){}
    };

    struct TwoParamFunctor
    {
        void operator()(ParamType, ParamType){}
    };

    struct ThreeParamFunctor
    {
        void operator()(ParamType, ParamType, ParamType){}
    };

    struct FourParamFunctor
    {
        void operator()(ParamType, ParamType, ParamType, ParamType){}
    };

    struct FiveParamFunctor
    {
        void operator()(ParamType, ParamType, ParamType, ParamType, ParamType){}
    };

    STATIC_ASSERT((!ttl::models<ttl::invocable, NotInvocableFunctor>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, NotInvocableFunctor, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, NotInvocableFunctor, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, NotInvocableFunctor, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, NotInvocableFunctor, ParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, NotInvocableFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value));

    STATIC_ASSERT((ttl::models<ttl::invocable, ZeroParamFunctor>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, ZeroParamFunctor, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, ZeroParamFunctor, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, ZeroParamFunctor, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, ZeroParamFunctor, ParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, ZeroParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value));
    
    STATIC_ASSERT((!ttl::models<ttl::invocable, OneParamFunctor>::value));
    STATIC_ASSERT((ttl::models<ttl::invocable, OneParamFunctor, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, OneParamFunctor, NotParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, OneParamFunctor, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, OneParamFunctor, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, OneParamFunctor, ParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, OneParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value));
    
    STATIC_ASSERT((!ttl::models<ttl::invocable, TwoParamFunctor>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, TwoParamFunctor, ParamType>::value));
    STATIC_ASSERT((ttl::models<ttl::invocable, TwoParamFunctor, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, TwoParamFunctor, NotParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, TwoParamFunctor, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, TwoParamFunctor, ParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, TwoParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value));
    
    STATIC_ASSERT((!ttl::models<ttl::invocable, ThreeParamFunctor>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, ThreeParamFunctor, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, ThreeParamFunctor, ParamType, ParamType>::value));
    STATIC_ASSERT((ttl::models<ttl::invocable, ThreeParamFunctor, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, ThreeParamFunctor, NotParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, ThreeParamFunctor, ParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, ThreeParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value));
    
    STATIC_ASSERT((!ttl::models<ttl::invocable, FourParamFunctor>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, FourParamFunctor, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, FourParamFunctor, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, FourParamFunctor, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((ttl::models<ttl::invocable, FourParamFunctor, ParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, FourParamFunctor, NotParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, FourParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value));
    
    STATIC_ASSERT((!ttl::models<ttl::invocable, FiveParamFunctor>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, FiveParamFunctor, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, FiveParamFunctor, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, FiveParamFunctor, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, FiveParamFunctor, ParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((ttl::models<ttl::invocable, FiveParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, FiveParamFunctor, NotParamType, ParamType, ParamType, ParamType, ParamType>::value));
    
    STATIC_ASSERT((ttl::models<ttl::invocable, __decltype(ZeroParamFunction)>::value));
    STATIC_ASSERT((ttl::models<ttl::invocable, __decltype(OneParamFunction), ParamType>::value));
    STATIC_ASSERT((ttl::models<ttl::invocable, __decltype(TwoParamFunction), ParamType, ParamType>::value));
    STATIC_ASSERT((ttl::models<ttl::invocable, __decltype(ThreeParamFunction), ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((ttl::models<ttl::invocable, __decltype(FourParamFunction), ParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((ttl::models<ttl::invocable, __decltype(FiveParamFunction), ParamType, ParamType, ParamType, ParamType, ParamType>::value));
    
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(OneParamFunction)>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(TwoParamFunction)>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(ThreeParamFunction)>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(FourParamFunction)>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(FiveParamFunction)>::value));
    
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(ZeroParamFunction), NotParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(OneParamFunction), NotParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(TwoParamFunction), NotParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(ThreeParamFunction), NotParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(FourParamFunction), NotParamType, ParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(FiveParamFunction), NotParamType, ParamType, ParamType, ParamType, ParamType>::value));
    
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(TwoParamFunction), ParamType, NotParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(ThreeParamFunction), ParamType, NotParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(FourParamFunction), ParamType, NotParamType, ParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(FiveParamFunction), ParamType, NotParamType, ParamType, ParamType, ParamType>::value));
    
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(ThreeParamFunction), ParamType, ParamType, NotParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(FourParamFunction), ParamType, ParamType, NotParamType, ParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(FiveParamFunction), ParamType, ParamType, NotParamType, ParamType, ParamType>::value));
    
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(FourParamFunction), ParamType, ParamType, ParamType, NotParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(FiveParamFunction), ParamType, ParamType, ParamType, NotParamType, ParamType>::value));
    
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(FiveParamFunction), ParamType, ParamType, ParamType, ParamType, NotParamType>::value));
    
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(OneParamFunction), NotParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(TwoParamFunction), NotParamType, NotParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(ThreeParamFunction), NotParamType, NotParamType, NotParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(FourParamFunction), NotParamType, NotParamType, NotParamType, NotParamType>::value));
    STATIC_ASSERT((!ttl::models<ttl::invocable, __decltype(FiveParamFunction), NotParamType, NotParamType, NotParamType, NotParamType, NotParamType>::value));

}