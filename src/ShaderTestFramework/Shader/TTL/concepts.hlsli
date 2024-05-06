
#ifndef TTL_CONCEPTS_HEADER
#define TTL_CONCEPTS_HEADER

#include "/Test/TTL/models.hlsli"

namespace ttl_detail
{
    template<typename T, uint N>
    static ttl::integral_constant<uint, N> array_len(T In[N])
    {
        return (ttl::integral_constant<uint, N>)0;
    }
}

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

    struct string_literal
    {
        template<typename T>
        __decltype(
            models_if<(__decltype(ttl_detail::array_len(declval<T>()))::value > 0)>()
        ) requires();
    };
}

#endif
