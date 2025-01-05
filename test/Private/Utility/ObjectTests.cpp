
#include <concepts>
#include <Utility/Object.h>

namespace stf::ObjectTests::ObjectClassTests
{
    static_assert(!std::movable<Object>, "Expected Object to not be movable");
    static_assert(!std::copyable<Object>, "Expected Object to not be copyable");
    static_assert(!std::default_initializable<Object>, "Expected Object to not be default constructible");

    static_assert(std::has_virtual_destructor_v<Object>, "Expected Object to have a virtual destructor");
    static_assert(std::constructible_from<Object, ObjectToken>, "Expected an Object to be constructible from an ObjectToken");
}

namespace stf::ObjectTests::ObjectTypeConceptTests
{
    struct NotInheritedFromObject {};
    struct NotInheritedFromObjectTakesObjectToken { NotInheritedFromObjectTakesObjectToken(ObjectToken In) {} };
    struct PrivateInherit : private Object { PrivateInherit(ObjectToken In) : Object(In) {} };
    struct PublicInherit : public Object { PublicInherit(ObjectToken In) : Object(In) {} };

    static_assert(!ObjectType<Object>, "Expected Object itself to not pass the ObjectType concept");
    static_assert(!ObjectType<NotInheritedFromObject>, "Expected a class that does not inherit from Object to not pass the ObjectType concept");
    static_assert(!ObjectType<NotInheritedFromObjectTakesObjectToken>, "Expected a class that does not inherit from Object but takes an ObjectToken to construct to not pass the ObjectType concept");
    static_assert(!ObjectType<PrivateInherit>, "Expected Private inheritance to not pass the ObjectType concept");
    static_assert(ObjectType<PublicInherit>, "Expected Public inheritance to pass the ObjectType concept");
}

namespace stf::ObjectTests::ObjectTypeConstructionTests
{
    struct NotInheritedFromObject {};
    struct NotInheritedFromObjectTakesObjectToken { NotInheritedFromObjectTakesObjectToken(ObjectToken In) {} };
    struct PrivateInherit : private Object { PrivateInherit(ObjectToken In) : Object(In) {} };
    struct PublicInherit : public Object { PublicInherit(ObjectToken In) : Object(In) {} };

    template<typename T>
    concept TestConstructibleWithObjectNew = requires()
    {
        { Object::New<T>() } -> std::same_as<SharedPtr<T>>;
    };

    static_assert(!TestConstructibleWithObjectNew<Object>, "Expected Object itself to not be constructible using Object::New");
    static_assert(!TestConstructibleWithObjectNew<NotInheritedFromObject>, "Expected a class that does not inherit from Object to not be constructible using Object::New");
    static_assert(!TestConstructibleWithObjectNew<NotInheritedFromObjectTakesObjectToken>, "Expected a class that does not inherit from Object but takes an ObjectToken to construct to not be constructible using Object::New");
    static_assert(!TestConstructibleWithObjectNew<PrivateInherit>, "Expected Private inheritance to not be constructible using Object::New");
    static_assert(TestConstructibleWithObjectNew<PublicInherit>, "Expected Public inheritance to be constructible using Object::New");
}