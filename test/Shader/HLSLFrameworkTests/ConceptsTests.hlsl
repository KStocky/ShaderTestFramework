
#include "/Test/TTL/concepts.hlsli"

namespace EqualityComparableWithTests
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

    struct F
    {
        bool operator==(F In)
        {
            return true;
        }

        bool operator!=(F In)
        {
            return false;
        }
    };

    _Static_assert(!ttl::models<ttl::equality_comparable_with, A, A>::value);
    _Static_assert(!ttl::models<ttl::equality_comparable_with, A, B>::value);
    _Static_assert(ttl::models<ttl::equality_comparable_with, B, A>::value);
    _Static_assert(!ttl::models<ttl::equality_comparable_with, C, A>::value);
    _Static_assert(!ttl::models<ttl::equality_comparable_with, D, A>::value);
    _Static_assert(!ttl::models<ttl::equality_comparable_with, E, A>::value);
    _Static_assert(ttl::models<ttl::equality_comparable_with, F, F>::value);
}

namespace EqualityComparableTests
{
    struct A
    {

    };

    struct B
    {
        bool operator==(B In)
        {
            return true;
        }

        bool operator!=(B In)
        {
            return false;
        }
    };

    struct C
    {
        int operator==(C In)
        {
            return 1;
        }

        bool operator!=(C In)
        {
            return false;
        }
    };

    struct D
    {
        bool operator==(D In)
        {
            return true;
        }

        int operator!=(D In)
        {
            return 1;
        }
    };

    struct E
    {
        int operator==(E In)
        {
            return 1;
        }

        int operator!=(E In)
        {
            return 1;
        }
    };

    _Static_assert(!ttl::models<ttl::equality_comparable, A>::value);
    _Static_assert(ttl::models<ttl::equality_comparable, B>::value);
    _Static_assert(!ttl::models<ttl::equality_comparable, C>::value);
    _Static_assert(!ttl::models<ttl::equality_comparable, D>::value);
    _Static_assert(!ttl::models<ttl::equality_comparable, E>::value);
}

namespace StringLiteralTests
{
    struct A
    {
    };

    _Static_assert(!ttl::models<ttl::string_literal, A[6]>::value);
    _Static_assert(!ttl::models<ttl::string_literal, A>::value);

    _Static_assert(ttl::models<ttl::string_literal, __decltype("Hello")>::value);
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

    _Static_assert(!ttl::models<ttl::invocable_functor, NotInvocableFunctor>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, NotInvocableFunctor, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, NotInvocableFunctor, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, NotInvocableFunctor, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, NotInvocableFunctor, ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, NotInvocableFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value);

    _Static_assert( ttl::models<ttl::invocable_functor, ZeroParamFunctor>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, ZeroParamFunctor, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, ZeroParamFunctor, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, ZeroParamFunctor, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, ZeroParamFunctor, ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, ZeroParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value);

    _Static_assert(!ttl::models<ttl::invocable_functor, OneParamFunctor>::value);
    _Static_assert(ttl::models<ttl::invocable_functor, OneParamFunctor, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, OneParamFunctor, NotParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, OneParamFunctor, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, OneParamFunctor, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, OneParamFunctor, ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, OneParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value);

    _Static_assert(!ttl::models<ttl::invocable_functor, TwoParamFunctor>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, TwoParamFunctor, ParamType>::value);
    _Static_assert(ttl::models<ttl::invocable_functor, TwoParamFunctor, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, TwoParamFunctor, NotParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, TwoParamFunctor, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, TwoParamFunctor, ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, TwoParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value);

    _Static_assert(!ttl::models<ttl::invocable_functor, ThreeParamFunctor>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, ThreeParamFunctor, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, ThreeParamFunctor, ParamType, ParamType>::value);
    _Static_assert(ttl::models<ttl::invocable_functor, ThreeParamFunctor, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, ThreeParamFunctor, NotParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, ThreeParamFunctor, ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, ThreeParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value);

    _Static_assert(!ttl::models<ttl::invocable_functor, FourParamFunctor>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, FourParamFunctor, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, FourParamFunctor, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, FourParamFunctor, ParamType, ParamType, ParamType>::value);
    _Static_assert(ttl::models<ttl::invocable_functor, FourParamFunctor, ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, FourParamFunctor, NotParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, FourParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value);

    _Static_assert(!ttl::models<ttl::invocable_functor, FiveParamFunctor>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, FiveParamFunctor, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, FiveParamFunctor, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, FiveParamFunctor, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, FiveParamFunctor, ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(ttl::models<ttl::invocable_functor, FiveParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable_functor, FiveParamFunctor, NotParamType, ParamType, ParamType, ParamType, ParamType>::value);
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

    _Static_assert(!ttl::models<ttl::invocable, NotInvocableFunctor>::value);
    _Static_assert(!ttl::models<ttl::invocable, NotInvocableFunctor, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, NotInvocableFunctor, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, NotInvocableFunctor, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, NotInvocableFunctor, ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, NotInvocableFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value);

    _Static_assert(ttl::models<ttl::invocable, ZeroParamFunctor>::value);
    _Static_assert(!ttl::models<ttl::invocable, ZeroParamFunctor, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, ZeroParamFunctor, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, ZeroParamFunctor, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, ZeroParamFunctor, ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, ZeroParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value);
    
    _Static_assert(!ttl::models<ttl::invocable, OneParamFunctor>::value);
    _Static_assert(ttl::models<ttl::invocable, OneParamFunctor, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, OneParamFunctor, NotParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, OneParamFunctor, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, OneParamFunctor, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, OneParamFunctor, ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, OneParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value);
    
    _Static_assert(!ttl::models<ttl::invocable, TwoParamFunctor>::value);
    _Static_assert(!ttl::models<ttl::invocable, TwoParamFunctor, ParamType>::value);
    _Static_assert(ttl::models<ttl::invocable, TwoParamFunctor, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, TwoParamFunctor, NotParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, TwoParamFunctor, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, TwoParamFunctor, ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, TwoParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value);
    
    _Static_assert(!ttl::models<ttl::invocable, ThreeParamFunctor>::value);
    _Static_assert(!ttl::models<ttl::invocable, ThreeParamFunctor, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, ThreeParamFunctor, ParamType, ParamType>::value);
    _Static_assert(ttl::models<ttl::invocable, ThreeParamFunctor, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, ThreeParamFunctor, NotParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, ThreeParamFunctor, ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, ThreeParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value);
    
    _Static_assert(!ttl::models<ttl::invocable, FourParamFunctor>::value);
    _Static_assert(!ttl::models<ttl::invocable, FourParamFunctor, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, FourParamFunctor, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, FourParamFunctor, ParamType, ParamType, ParamType>::value);
    _Static_assert(ttl::models<ttl::invocable, FourParamFunctor, ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, FourParamFunctor, NotParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, FourParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value);
    
    _Static_assert(!ttl::models<ttl::invocable, FiveParamFunctor>::value);
    _Static_assert(!ttl::models<ttl::invocable, FiveParamFunctor, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, FiveParamFunctor, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, FiveParamFunctor, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, FiveParamFunctor, ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(ttl::models<ttl::invocable, FiveParamFunctor, ParamType, ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, FiveParamFunctor, NotParamType, ParamType, ParamType, ParamType, ParamType>::value);
    
    _Static_assert(ttl::models<ttl::invocable, __decltype(ZeroParamFunction)>::value);
    _Static_assert(ttl::models<ttl::invocable, __decltype(OneParamFunction), ParamType>::value);
    _Static_assert(ttl::models<ttl::invocable, __decltype(TwoParamFunction), ParamType, ParamType>::value);
    _Static_assert(ttl::models<ttl::invocable, __decltype(ThreeParamFunction), ParamType, ParamType, ParamType>::value);
    _Static_assert(ttl::models<ttl::invocable, __decltype(FourParamFunction), ParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(ttl::models<ttl::invocable, __decltype(FiveParamFunction), ParamType, ParamType, ParamType, ParamType, ParamType>::value);
    
    _Static_assert(!ttl::models<ttl::invocable, __decltype(OneParamFunction)>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(TwoParamFunction)>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(ThreeParamFunction)>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(FourParamFunction)>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(FiveParamFunction)>::value);
    
    _Static_assert(!ttl::models<ttl::invocable, __decltype(ZeroParamFunction), NotParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(OneParamFunction), NotParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(TwoParamFunction), NotParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(ThreeParamFunction), NotParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(FourParamFunction), NotParamType, ParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(FiveParamFunction), NotParamType, ParamType, ParamType, ParamType, ParamType>::value);
    
    _Static_assert(!ttl::models<ttl::invocable, __decltype(TwoParamFunction), ParamType, NotParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(ThreeParamFunction), ParamType, NotParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(FourParamFunction), ParamType, NotParamType, ParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(FiveParamFunction), ParamType, NotParamType, ParamType, ParamType, ParamType>::value);
    
    _Static_assert(!ttl::models<ttl::invocable, __decltype(ThreeParamFunction), ParamType, ParamType, NotParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(FourParamFunction), ParamType, ParamType, NotParamType, ParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(FiveParamFunction), ParamType, ParamType, NotParamType, ParamType, ParamType>::value);
    
    _Static_assert(!ttl::models<ttl::invocable, __decltype(FourParamFunction), ParamType, ParamType, ParamType, NotParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(FiveParamFunction), ParamType, ParamType, ParamType, NotParamType, ParamType>::value);
    
    _Static_assert(!ttl::models<ttl::invocable, __decltype(FiveParamFunction), ParamType, ParamType, ParamType, ParamType, NotParamType>::value);
    
    _Static_assert(!ttl::models<ttl::invocable, __decltype(OneParamFunction), NotParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(TwoParamFunction), NotParamType, NotParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(ThreeParamFunction), NotParamType, NotParamType, NotParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(FourParamFunction), NotParamType, NotParamType, NotParamType, NotParamType>::value);
    _Static_assert(!ttl::models<ttl::invocable, __decltype(FiveParamFunction), NotParamType, NotParamType, NotParamType, NotParamType, NotParamType>::value);

}