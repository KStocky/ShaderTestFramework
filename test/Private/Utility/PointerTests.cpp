
#include <Utility/Pointer.h>

namespace stf::MakeUniqueTests
{
    template<typename T, typename... Ts>
    concept TestMakeUnique = requires(Ts... InArgs)
    {
        { MakeUnique<T>(InArgs...) } -> std::same_as<UniquePtr<T>>;
    };

    struct ParamA {};

    struct ParamB {};

    struct OnlyDefault
    {
        OnlyDefault() {}
    };

    struct TakesA
    {
        TakesA(ParamA) {}
    };

    struct TakesB
    {
        TakesB(ParamB) {}
    };

    struct TakesAThenB
    {
        TakesAThenB(ParamA, ParamB) {}
    };

    struct TakesBThenA
    {
        TakesBThenA(ParamB, ParamA) {}
    };

    static_assert(TestMakeUnique<OnlyDefault>);
    static_assert(!TestMakeUnique<OnlyDefault, ParamA>);
    static_assert(!TestMakeUnique<OnlyDefault, ParamB>);
    static_assert(!TestMakeUnique<OnlyDefault, ParamA, ParamB>);
    static_assert(!TestMakeUnique<OnlyDefault, ParamB, ParamA>);

    static_assert(!TestMakeUnique<TakesA>);
    static_assert(TestMakeUnique<TakesA, ParamA>);
    static_assert(!TestMakeUnique<TakesA, ParamB>);
    static_assert(!TestMakeUnique<TakesA, ParamA, ParamB>);
    static_assert(!TestMakeUnique<TakesA, ParamB, ParamA>);

    static_assert(!TestMakeUnique<TakesB>);
    static_assert(!TestMakeUnique<TakesB, ParamA>);
    static_assert(TestMakeUnique<TakesB, ParamB>);
    static_assert(!TestMakeUnique<TakesB, ParamA, ParamB>);
    static_assert(!TestMakeUnique<TakesB, ParamB, ParamA>);

    static_assert(!TestMakeUnique<TakesAThenB>);
    static_assert(!TestMakeUnique<TakesAThenB, ParamA>);
    static_assert(!TestMakeUnique<TakesAThenB, ParamB>);
    static_assert(TestMakeUnique<TakesAThenB, ParamA, ParamB>);
    static_assert(!TestMakeUnique<TakesAThenB, ParamB, ParamA>);

    static_assert(!TestMakeUnique<TakesBThenA>);
    static_assert(!TestMakeUnique<TakesBThenA, ParamA>);
    static_assert(!TestMakeUnique<TakesBThenA, ParamB>);
    static_assert(!TestMakeUnique<TakesBThenA, ParamA, ParamB>);
    static_assert(TestMakeUnique<TakesBThenA, ParamB, ParamA>);
}

namespace stf::MakeSharedTests
{
    template<typename T, typename... Ts>
    concept TestMakeShared = requires(Ts... InArgs)
    {
        { MakeShared<T>(InArgs...) } -> std::same_as<SharedPtr<T>>;
    };

    struct ParamA {};

    struct ParamB {};

    struct OnlyDefault
    {
        OnlyDefault() {}
    };

    struct TakesA
    {
        TakesA(ParamA) {}
    };

    struct TakesB
    {
        TakesB(ParamB) {}
    };

    struct TakesAThenB
    {
        TakesAThenB(ParamA, ParamB) {}
    };

    struct TakesBThenA
    {
        TakesBThenA(ParamB, ParamA) {}
    };

    static_assert(TestMakeShared<OnlyDefault>);
    static_assert(!TestMakeShared<OnlyDefault, ParamA>);
    static_assert(!TestMakeShared<OnlyDefault, ParamB>);
    static_assert(!TestMakeShared<OnlyDefault, ParamA, ParamB>);
    static_assert(!TestMakeShared<OnlyDefault, ParamB, ParamA>);

    static_assert(!TestMakeShared<TakesA>);
    static_assert(TestMakeShared<TakesA, ParamA>);
    static_assert(!TestMakeShared<TakesA, ParamB>);
    static_assert(!TestMakeShared<TakesA, ParamA, ParamB>);
    static_assert(!TestMakeShared<TakesA, ParamB, ParamA>);

    static_assert(!TestMakeShared<TakesB>);
    static_assert(!TestMakeShared<TakesB, ParamA>);
    static_assert(TestMakeShared<TakesB, ParamB>);
    static_assert(!TestMakeShared<TakesB, ParamA, ParamB>);
    static_assert(!TestMakeShared<TakesB, ParamB, ParamA>);

    static_assert(!TestMakeShared<TakesAThenB>);
    static_assert(!TestMakeShared<TakesAThenB, ParamA>);
    static_assert(!TestMakeShared<TakesAThenB, ParamB>);
    static_assert(TestMakeShared<TakesAThenB, ParamA, ParamB>);
    static_assert(!TestMakeShared<TakesAThenB, ParamB, ParamA>);

    static_assert(!TestMakeShared<TakesBThenA>);
    static_assert(!TestMakeShared<TakesBThenA, ParamA>);
    static_assert(!TestMakeShared<TakesBThenA, ParamB>);
    static_assert(!TestMakeShared<TakesBThenA, ParamA, ParamB>);
    static_assert(TestMakeShared<TakesBThenA, ParamB, ParamA>);
}