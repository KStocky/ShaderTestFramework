
#include <Stats/ScopedObject.h>
#include <Utility/Concepts.h>

#include <catch2/catch_test_macros.hpp>

namespace ScopedObjectCompileTests
{
    using CallableType = decltype([]() {});
    
    struct NonCallableType {};

    static_assert(InstantiatableFrom<ScopedObject, CallableType>);
    static_assert(!InstantiatableFrom<ScopedObject, NonCallableType>);
    static_assert(!DefaultConstructibleType<ScopedObject<CallableType>>);

    static_assert(!std::constructible_from<ScopedObject<CallableType>, CallableType>);
    static_assert(!std::constructible_from<ScopedObject<CallableType>, NonCallableType>);

    static_assert(!std::copyable<ScopedObject<CallableType>>);
    static_assert(!std::movable<ScopedObject<CallableType>>);

    static_assert(std::constructible_from<ScopedObject<CallableType>, CallableType, CallableType>);

    static_assert(!Newable<ScopedObject<CallableType>, CallableType, CallableType>);
}
