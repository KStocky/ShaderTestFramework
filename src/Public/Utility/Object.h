#pragma once

namespace stf
{
    class Object
    {
    public:
        Object() = default;
        Object(const Object&) = delete;
        Object(Object&&) = delete;

        Object& operator=(const Object&) = delete;
        Object& operator=(Object&&) = delete;
    };
}