#include "D3D12/CommandEngine.h"
#include "Utility/Lambda.h"

namespace CommandEngineFuncTypeTests
{
    using namespace stf;
    template<typename... T>
    struct CallableType
    {
        void operator()(T...) {}
    };

    struct NonCallableType
    {
    };

    template<typename... T>
    using FreeFuncType = void(T...);

    template<typename... T>
    using LambdaType = decltype([](T...) {});

    template<typename... T>
    using EngineLambdaType = decltype(Lambda([](T...) {}));

    static_assert(!CommandEngineFuncType<CallableType<ScopedCommandContext&>>);
    static_assert(!CommandEngineFuncType<FreeFuncType<ScopedCommandContext&>>);
    static_assert(!CommandEngineFuncType<LambdaType<ScopedCommandContext&>>);
    static_assert(CommandEngineFuncType<EngineLambdaType<ScopedCommandContext&>>);

    static_assert(!CommandEngineFuncType<CallableType<ScopedCommandContext&, NonCallableType>>);
    static_assert(!CommandEngineFuncType<FreeFuncType<ScopedCommandContext&, NonCallableType>>);
    static_assert(!CommandEngineFuncType<LambdaType<ScopedCommandContext&, NonCallableType>>);
    static_assert(!CommandEngineFuncType<EngineLambdaType<ScopedCommandContext&, NonCallableType>>);

    static_assert(!CommandEngineFuncType<CallableType<ScopedCommandContext>>);
    static_assert(!CommandEngineFuncType<FreeFuncType<ScopedCommandContext>>);
    static_assert(!CommandEngineFuncType<LambdaType<ScopedCommandContext>>);
    static_assert(!CommandEngineFuncType<EngineLambdaType<ScopedCommandContext>>);

    static_assert(!CommandEngineFuncType<CallableType<const ScopedCommandContext&>>);
    static_assert(!CommandEngineFuncType<FreeFuncType<const ScopedCommandContext&>>);
    static_assert(!CommandEngineFuncType<LambdaType<const ScopedCommandContext&>>);
    static_assert(!CommandEngineFuncType<EngineLambdaType<const ScopedCommandContext&>>);

    static_assert(!CommandEngineFuncType<CallableType<NonCallableType&>>);
    static_assert(!CommandEngineFuncType<FreeFuncType<NonCallableType&>>);
    static_assert(!CommandEngineFuncType<LambdaType<NonCallableType&>>);
    static_assert(!CommandEngineFuncType<EngineLambdaType<NonCallableType&>>);
}

namespace ExecuteLambdaTypeTests
{
    using namespace stf;
    template<typename... T>
    struct CallableType
    {
        void operator()(T...) {}
    };

    struct NonCallableType
    {
    };

    template<typename... T>
    using FreeFuncType = void(T...);

    template<typename... T>
    using LambdaType = decltype([](T...) {});

    template<typename... T>
    using EngineLambdaType = decltype(Lambda([](T...){}));

    static_assert(ExecuteLambdaType<CallableType<ScopedCommandContext&>>);
    static_assert(ExecuteLambdaType<FreeFuncType<ScopedCommandContext&>>);
    static_assert(ExecuteLambdaType<LambdaType<ScopedCommandContext&>>);
    static_assert(!ExecuteLambdaType<EngineLambdaType<ScopedCommandContext&>>);

    static_assert(!ExecuteLambdaType<CallableType<ScopedCommandContext&, NonCallableType>>);
    static_assert(!ExecuteLambdaType<FreeFuncType<ScopedCommandContext&, NonCallableType>>);
    static_assert(!ExecuteLambdaType<LambdaType<ScopedCommandContext&, NonCallableType>>);
    static_assert(!ExecuteLambdaType<EngineLambdaType<ScopedCommandContext&, NonCallableType>>);

    static_assert(!ExecuteLambdaType<CallableType<ScopedCommandContext>>);
    static_assert(!ExecuteLambdaType<FreeFuncType<ScopedCommandContext>>);
    static_assert(!ExecuteLambdaType<LambdaType<ScopedCommandContext>>);
    static_assert(!ExecuteLambdaType<EngineLambdaType<ScopedCommandContext>>);

    static_assert(!ExecuteLambdaType<CallableType<const ScopedCommandContext&>>);
    static_assert(!ExecuteLambdaType<FreeFuncType<const ScopedCommandContext&>>);
    static_assert(!ExecuteLambdaType<LambdaType<const ScopedCommandContext&>>);
    static_assert(!ExecuteLambdaType<EngineLambdaType<const ScopedCommandContext&>>);

    static_assert(!ExecuteLambdaType<CallableType<NonCallableType&>>);
    static_assert(!ExecuteLambdaType<FreeFuncType<NonCallableType&>>);
    static_assert(!ExecuteLambdaType<LambdaType<NonCallableType&>>);
    static_assert(!ExecuteLambdaType<EngineLambdaType<NonCallableType&>>);
}