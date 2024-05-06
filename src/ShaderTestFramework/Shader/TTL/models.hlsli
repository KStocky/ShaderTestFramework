#ifndef TTL_MODELS_HEADER
#define TTL_MODELS_HEADER

#include "/Test/TTL/type_traits.hlsli"

namespace ttl_detail
{
    struct null_type{};

    template<typename Concept, 
        typename Arg0,
        typename Arg1, typename Arg2, 
        typename Arg3, typename Arg4, typename Arg5, 
        typename = void
        >
    struct models_resolver
    {
        static const bool value = false;
    };

    template<typename Concept, typename Arg0>
    struct models_resolver
        <Concept, 
            Arg0, null_type, null_type, null_type, null_type, null_type,
            ttl::void_t<__decltype(ttl::declval<Concept>().template requires<Arg0>())>
        >
    {
        static const bool value = true;
    };

    template<typename Concept, typename Arg0, typename Arg1>
    struct models_resolver
        <Concept, 
            Arg0, Arg1, null_type, null_type, null_type, null_type,
            ttl::void_t<__decltype(ttl::declval<Concept>().template requires<Arg0, Arg1>())>
        >
    {
        static const bool value = true;
    };

    template<typename Concept, typename Arg0, typename Arg1, typename Arg2>
    struct models_resolver
        <Concept, 
            Arg0, Arg1, Arg2, null_type, null_type, null_type,
            ttl::void_t<__decltype(ttl::declval<Concept>().template requires<Arg0, Arg1, Arg2>())>
        >
    {
        static const bool value = true;
    };

    template<typename Concept, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
    struct models_resolver
        <Concept, 
            Arg0, Arg1, Arg2, Arg3, null_type, null_type,
            ttl::void_t<__decltype(ttl::declval<Concept>().template requires<Arg0, Arg1, Arg2, Arg3>())>
        >
    {
        static const bool value = true;
    };

    template<typename Concept, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    struct models_resolver
        <Concept, 
            Arg0, Arg1, Arg2, Arg3, Arg4, null_type,
            ttl::void_t<__decltype(ttl::declval<Concept>().template requires<Arg0, Arg1, Arg2, Arg3, Arg4>())>
        >
    {
        static const bool value = true;
    };

    template<typename Concept, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    struct models_resolver
        <Concept, 
            Arg0, Arg1, Arg2, Arg3, Arg4, Arg5,
            ttl::void_t<__decltype(ttl::declval<Concept>().template requires<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5>())>
        >
    {
        static const bool value = true;
    };
}

namespace ttl
{
    template <
        typename Concept, 
        typename Arg0, typename Arg1 = ttl_detail::null_type, typename Arg2 = ttl_detail::null_type,
        typename Arg3 = ttl_detail::null_type, typename Arg4 = ttl_detail::null_type, typename Arg5 = ttl_detail::null_type
    >
    struct models
    {
        static const bool value = 
            ttl_detail::models_resolver<Concept, Arg0, Arg1, Arg2, Arg3, Arg4, Arg5>::value;
    };

    template<bool Cond> struct models_if{};
    template<> struct models_if<true> : integral_constant<bool, true>{};

    template<typename T, typename U> struct models_if_same{};
    template<typename T> struct models_if_same<T, T> : integral_constant<bool, true>{};
}

#endif