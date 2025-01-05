#pragma once

#include "Utility/Concepts.h"
#include "Utility/Pointer.h"

namespace stf
{
    class ObjectToken
    {
        friend class Object;
        ObjectToken() = default;
    };

    class Object;

    template<typename T, typename... Params>
    concept ObjectType =
        std::derived_from<T, Object> &&
        std::constructible_from<T, ObjectToken, Params...> &&
        !std::same_as<T, Object>;        

    class Object : public SharedFromThis<Object>
    {
    public:
        Object() = delete;
        explicit Object(ObjectToken) {};
        Object(const Object&) = delete;
        Object(Object&&) = delete;

        Object& operator=(const Object&) = delete;
        Object& operator=(Object&&) = delete;

        virtual ~Object() noexcept {}

        template<typename T, typename... ParamTypes>
            requires ObjectType<T, ParamTypes...>
        static SharedPtr<T> New(ParamTypes&&... InArgs)
        {
            return MakeShared<T>(ObjectToken{}, std::forward<ParamTypes>(InArgs)...);
        }
    };
}