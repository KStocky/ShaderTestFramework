
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