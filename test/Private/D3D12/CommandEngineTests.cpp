#include "D3D12/CommandEngine.h"
#include <Utility/Error.h>
#include "Utility/Lambda.h"

namespace CommandEngineFuncTypeTests
{
    using namespace stf;
    template<typename Ret, typename... T>
    struct CallableType
    {
        Ret operator()(T...) { return Ret{}; }
    };

    struct NonCallableType
    {
    };

    template<typename Ret, typename... T>
    using FreeFuncType = Ret(T...);

    template<typename Ret, typename... T>
    using LambdaType = decltype([](T...) { return Ret{}; });

    template<typename Ret, typename... T>
    using EngineLambdaType = decltype(Lambda([](T...) { return Ret{}; }));

    using ValidRet = ExpectedError<void>;
    using InvalidRet = i32;

    static_assert(!CommandEngineFuncType<CallableType<ValidRet, ScopedCommandContext&>>);
    static_assert(!CommandEngineFuncType<FreeFuncType<ValidRet, ScopedCommandContext&>>);
    static_assert(!CommandEngineFuncType<LambdaType<ValidRet, ScopedCommandContext&>>);
    static_assert(CommandEngineFuncType<EngineLambdaType<ValidRet, ScopedCommandContext&>>);

    static_assert(!CommandEngineFuncType<CallableType<ValidRet, ScopedCommandContext&, NonCallableType>>);
    static_assert(!CommandEngineFuncType<FreeFuncType<ValidRet, ScopedCommandContext&, NonCallableType>>);
    static_assert(!CommandEngineFuncType<LambdaType<ValidRet, ScopedCommandContext&, NonCallableType>>);
    static_assert(!CommandEngineFuncType<EngineLambdaType<ValidRet, ScopedCommandContext&, NonCallableType>>);

    static_assert(!CommandEngineFuncType<CallableType<ValidRet, ScopedCommandContext>>);
    static_assert(!CommandEngineFuncType<FreeFuncType<ValidRet, ScopedCommandContext>>);
    static_assert(!CommandEngineFuncType<LambdaType<ValidRet, ScopedCommandContext>>);
    static_assert(!CommandEngineFuncType<EngineLambdaType<ValidRet, ScopedCommandContext>>);

    static_assert(!CommandEngineFuncType<CallableType<ValidRet, const ScopedCommandContext&>>);
    static_assert(!CommandEngineFuncType<FreeFuncType<ValidRet, const ScopedCommandContext&>>);
    static_assert(!CommandEngineFuncType<LambdaType<ValidRet, const ScopedCommandContext&>>);
    static_assert(!CommandEngineFuncType<EngineLambdaType<ValidRet, const ScopedCommandContext&>>);

    static_assert(!CommandEngineFuncType<CallableType<ValidRet, NonCallableType&>>);
    static_assert(!CommandEngineFuncType<FreeFuncType<ValidRet, NonCallableType&>>);
    static_assert(!CommandEngineFuncType<LambdaType<ValidRet, NonCallableType&>>);
    static_assert(!CommandEngineFuncType<EngineLambdaType<ValidRet, NonCallableType&>>);
}

namespace ExecuteLambdaTypeTests
{
    using namespace stf;
    template<typename Ret, typename... T>
    struct CallableType
    {
        Ret operator()(T...) { return Ret{}; }
    };

    struct NonCallableType
    {
    };

    template<typename Ret, typename... T>
    using FreeFuncType = Ret(T...);

    template<typename Ret, typename... T>
    using LambdaType = decltype([](T...) { return Ret{}; });

    template<typename Ret, typename... T>
    using EngineLambdaType = decltype(Lambda([](T...) { return Ret{}; }));

    using ValidRet = ExpectedError<void>;
    using InvalidRet = i32;

    static_assert(ExecuteLambdaType<CallableType<ValidRet, ScopedCommandContext&>>);
    static_assert(ExecuteLambdaType<FreeFuncType<ValidRet, ScopedCommandContext&>>);
    static_assert(ExecuteLambdaType<LambdaType<ValidRet, ScopedCommandContext&>>);
    static_assert(!ExecuteLambdaType<EngineLambdaType<ValidRet, ScopedCommandContext&>>);

    static_assert(!ExecuteLambdaType<CallableType<ValidRet, ScopedCommandContext&, NonCallableType>>);
    static_assert(!ExecuteLambdaType<FreeFuncType<ValidRet, ScopedCommandContext&, NonCallableType>>);
    static_assert(!ExecuteLambdaType<LambdaType<ValidRet, ScopedCommandContext&, NonCallableType>>);
    static_assert(!ExecuteLambdaType<EngineLambdaType<ValidRet, ScopedCommandContext&, NonCallableType>>);

    static_assert(!ExecuteLambdaType<CallableType<ValidRet, ScopedCommandContext>>);
    static_assert(!ExecuteLambdaType<FreeFuncType<ValidRet, ScopedCommandContext>>);
    static_assert(!ExecuteLambdaType<LambdaType<ValidRet, ScopedCommandContext>>);
    static_assert(!ExecuteLambdaType<EngineLambdaType<ValidRet, ScopedCommandContext>>);

    static_assert(!ExecuteLambdaType<CallableType<ValidRet, const ScopedCommandContext&>>);
    static_assert(!ExecuteLambdaType<FreeFuncType<ValidRet, const ScopedCommandContext&>>);
    static_assert(!ExecuteLambdaType<LambdaType<ValidRet, const ScopedCommandContext&>>);
    static_assert(!ExecuteLambdaType<EngineLambdaType<ValidRet, const ScopedCommandContext&>>);

    static_assert(!ExecuteLambdaType<CallableType<ValidRet, NonCallableType&>>);
    static_assert(!ExecuteLambdaType<FreeFuncType<ValidRet, NonCallableType&>>);
    static_assert(!ExecuteLambdaType<LambdaType<ValidRet, NonCallableType&>>);
    static_assert(!ExecuteLambdaType<EngineLambdaType<ValidRet, NonCallableType&>>);
}