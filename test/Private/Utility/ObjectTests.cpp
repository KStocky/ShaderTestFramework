
#include <Utility/Object.h>

#include <utility>

namespace ObjectTests
{
    template<typename T>
    concept NotObjectType = requires(T InA, T InB)
    {
        { T{ InA } };
        { T{ std::move(InB) } };
        { InA = InB };
        { InA = std::move(InB) };
    };

    template<typename T>
    concept ObjectType = !NotObjectType<T>;

    struct TestObject : private stf::Object {};

    static_assert(ObjectType<TestObject>, "Expected a class that inherits from stf::Object to conform to this concept");
}