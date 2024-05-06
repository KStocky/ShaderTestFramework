
#ifndef TTL_CONCEPTS_HEADER
#define TTL_CONCEPTS_HEADER

#include "/Test/TTL/models.hlsli"

namespace ttl
{
    struct equality_comparable
    {
        template<typename T, typename U>
        __decltype(
            models_if_same<bool, __decltype(declval<T>() == declval<U>())>(),
            models_if_same<bool, __decltype(declval<T>() != declval<U>())>()
        ) requires();
    };
}

#endif
