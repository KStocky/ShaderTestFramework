
#include "/Test/TTL/concepts.hlsli"
#include "/Test/TTL/static_assert.hlsli"

namespace EqualityComparableTests
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

    STATIC_ASSERT((!ttl::models<ttl::equality_comparable, A, A>::value));
    STATIC_ASSERT((!ttl::models<ttl::equality_comparable, A, B>::value));
    STATIC_ASSERT((ttl::models<ttl::equality_comparable, B, A>::value));
    STATIC_ASSERT((!ttl::models<ttl::equality_comparable, C, A>::value));
    STATIC_ASSERT((!ttl::models<ttl::equality_comparable, D, A>::value));
    STATIC_ASSERT((!ttl::models<ttl::equality_comparable, E, A>::value));
}

namespace IsStringTests
{
    struct A
    {
    };

    STATIC_ASSERT((!ttl::models<ttl::is_string, A[6]>::value));
    STATIC_ASSERT((!ttl::models<ttl::is_string, A>::value));

    STATIC_ASSERT((ttl::models<ttl::is_string, __decltype("Hello")>::value));
}