
#include "/Test/TTL/memory.hlsli"
#include "/Test/TTL/models.hlsli"
#include "/Test/TTL/static_assert.hlsli"

namespace ModelsTests
{
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
}

namespace ModelsIfTests
{
    struct TrueType : ttl::true_type{};
    struct FalseType : ttl::false_type{};

    struct IsTrueConcept
    {
        template<typename T>
        __decltype(
            ttl::models_if<T::value>()
        ) requires();
    };

    STATIC_ASSERT((ttl::models<IsTrueConcept, TrueType>::value));
    STATIC_ASSERT((!ttl::models<IsTrueConcept, FalseType>::value));
} 

namespace ModelsIfSameTests
{
    struct ReturnType1{};
    struct ReturnType2{};

    struct FuncReturnVoid
    {
        void operator()(){}
    };

    struct FuncReturnType1
    {
        ReturnType1 operator()()
        {
            return ttl::zero<ReturnType1>();
        }
    };

    struct FuncReturnType2
    {
        ReturnType2 operator()()
        {
            return ttl::zero<ReturnType2>();
        }
    };

    struct FuncReturnTypeConcept
    {
        template<typename T, typename Ret>
        __decltype(
            ttl::models_if_same<Ret, __decltype(ttl::declval<T>()())>()
        ) requires();
    };

    STATIC_ASSERT((ttl::models<FuncReturnTypeConcept, FuncReturnVoid, void>::value));
    STATIC_ASSERT((!ttl::models<FuncReturnTypeConcept, FuncReturnVoid, ReturnType1>::value));
    STATIC_ASSERT((!ttl::models<FuncReturnTypeConcept, FuncReturnVoid, ReturnType2>::value));

    STATIC_ASSERT((!ttl::models<FuncReturnTypeConcept, FuncReturnType1, void>::value));
    STATIC_ASSERT((ttl::models<FuncReturnTypeConcept, FuncReturnType1, ReturnType1>::value));
    STATIC_ASSERT((!ttl::models<FuncReturnTypeConcept, FuncReturnType1, ReturnType2>::value));

    STATIC_ASSERT((!ttl::models<FuncReturnTypeConcept, FuncReturnType2, void>::value));
    STATIC_ASSERT((!ttl::models<FuncReturnTypeConcept, FuncReturnType2, ReturnType1>::value));
    STATIC_ASSERT((ttl::models<FuncReturnTypeConcept, FuncReturnType2, ReturnType2>::value));
}

namespace ModelsRefinesTests
{
    struct HasFooConcept
    {
        template<typename T>
        __decltype(
            ttl::declval<T>().Foo()
        ) requires();
    };

    struct HasBarConcept
    {
        template<typename T>
        __decltype(
            ttl::declval<T>().Bar()
        ) requires();
    };

    struct HasFooAndBarConcept
    {
        template<typename T>
        __decltype(
            ttl::models_refines<HasFooConcept, T>(),
            ttl::models_refines<HasBarConcept, T>()
        ) requires();
    };

    struct HasFoo
    {
        void Foo();
    };

    struct HasBar
    {
        void Bar();
    };

    struct HasBoth
    {
        void Foo();
        void Bar();
    };

    struct HasNeither{};

    STATIC_ASSERT((!ttl::models<HasFooAndBarConcept, HasFoo>::value));
    STATIC_ASSERT((!ttl::models<HasFooAndBarConcept, HasBar>::value));
    STATIC_ASSERT((ttl::models<HasFooAndBarConcept, HasBoth>::value));
    STATIC_ASSERT((!ttl::models<HasFooAndBarConcept, HasNeither>::value));
}

namespace ModelsIfPredTests
{
    struct FooReturnsIntConcept
    {
        template<typename T>
        using is_same_int = ttl::is_same<int, T>;

        template<typename T>
        __decltype(
            ttl::models_if_pred<is_same_int, __decltype(ttl::declval<T>().Foo())>()
        ) requires();
    };

    struct FooReturnsFloatOrInt
    {
        template<typename T>
        struct FloatOrInt : ttl::integral_constant<bool, ttl::is_same<T, int>::value || ttl::is_same<T, float>::value>{};

        template<typename T>
        __decltype(
            ttl::models_if_pred<
                FloatOrInt, __decltype(ttl::declval<T>().Foo())
            >()
        ) requires();
    };

    struct NoFoo{};

    struct VoidFoo
    {
        void Foo();
    };

    struct IntFoo
    {
        int Foo();
    };

    struct FloatFoo
    {
        float Foo();
    };

    STATIC_ASSERT((!ttl::models<FooReturnsIntConcept, NoFoo>::value));
    STATIC_ASSERT((!ttl::models<FooReturnsIntConcept, VoidFoo>::value));
    STATIC_ASSERT((ttl::models<FooReturnsIntConcept, IntFoo>::value));

    STATIC_ASSERT((ttl::models<FooReturnsFloatOrInt, IntFoo>::value));
    STATIC_ASSERT((ttl::models<FooReturnsFloatOrInt, FloatFoo>::value));
    STATIC_ASSERT((!ttl::models<FooReturnsFloatOrInt, NoFoo>::value));
    STATIC_ASSERT((!ttl::models<FooReturnsFloatOrInt, VoidFoo>::value));
}