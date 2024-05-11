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
    struct models_resolver : ttl::false_type{};

    template<typename Concept, typename Arg0>
    struct models_resolver
        <Concept, 
            Arg0, null_type, null_type, null_type, null_type, null_type,
            ttl::void_t<
                __decltype(ttl::declval<Concept>().template requires<Arg0>()),
                typename ttl::enable_if<
                    !ttl::is_same<Arg0, null_type>::value
                >::type
            >
        > : ttl::true_type {};

    template<typename Concept, typename Arg0, typename Arg1>
    struct models_resolver
        <Concept, 
            Arg0, Arg1, null_type, null_type, null_type, null_type,
            ttl::void_t<
                __decltype(ttl::declval<Concept>().template requires<Arg0, Arg1>()),
                typename ttl::enable_if<
                    !ttl::is_same<Arg0, null_type>::value &&
                    !ttl::is_same<Arg1, null_type>::value
                >::type
            >
        > : ttl::true_type {};

    template<typename Concept, typename Arg0, typename Arg1, typename Arg2>
    struct models_resolver
        <Concept, 
            Arg0, Arg1, Arg2, null_type, null_type, null_type,
            ttl::void_t<
                __decltype(ttl::declval<Concept>().template requires<Arg0, Arg1, Arg2>()),
                typename ttl::enable_if<
                    !ttl::is_same<Arg0, null_type>::value &&
                    !ttl::is_same<Arg1, null_type>::value &&
                    !ttl::is_same<Arg2, null_type>::value
                >::type
            >
        > : ttl::true_type {};

    template<typename Concept, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
    struct models_resolver
        <Concept, 
            Arg0, Arg1, Arg2, Arg3, null_type, null_type,
            ttl::void_t<
                __decltype(ttl::declval<Concept>().template requires<Arg0, Arg1, Arg2, Arg3>()),
                typename ttl::enable_if<
                    !ttl::is_same<Arg0, null_type>::value &&
                    !ttl::is_same<Arg1, null_type>::value &&
                    !ttl::is_same<Arg2, null_type>::value &&
                    !ttl::is_same<Arg3, null_type>::value
                >::type
            >
        > : ttl::true_type {};

    template<typename Concept, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    struct models_resolver
        <Concept, 
            Arg0, Arg1, Arg2, Arg3, Arg4, null_type,
            ttl::void_t<
                __decltype(ttl::declval<Concept>().template requires<Arg0, Arg1, Arg2, Arg3, Arg4>()),
                typename ttl::enable_if<
                    !ttl::is_same<Arg0, null_type>::value &&
                    !ttl::is_same<Arg1, null_type>::value &&
                    !ttl::is_same<Arg2, null_type>::value &&
                    !ttl::is_same<Arg3, null_type>::value &&
                    !ttl::is_same<Arg4, null_type>::value
                >::type
            >
        > : ttl::true_type {};

    template<typename Concept, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
    struct models_resolver
        <Concept, 
            Arg0, Arg1, Arg2, Arg3, Arg4, Arg5,
            ttl::void_t<__decltype(ttl::declval<Concept>().template requires<Arg0, Arg1, Arg2, Arg3, Arg4, Arg5>()),
                typename ttl::enable_if<
                    !ttl::is_same<Arg0, null_type>::value &&
                    !ttl::is_same<Arg1, null_type>::value &&
                    !ttl::is_same<Arg2, null_type>::value &&
                    !ttl::is_same<Arg3, null_type>::value &&
                    !ttl::is_same<Arg4, null_type>::value &&
                    !ttl::is_same<Arg5, null_type>::value
                >::type
            >
        > : ttl::true_type {};
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

    template<bool Cond>
    typename enable_if<Cond>::type models_if();

    template<typename T, typename U>
    typename enable_if<is_same<T, U>::value>::type models_if_same(); 

    template<
        typename Concept, 
        typename Arg0, typename Arg1 = ttl_detail::null_type, typename Arg2 = ttl_detail::null_type,
        typename Arg3 = ttl_detail::null_type, typename Arg4 = ttl_detail::null_type, typename Arg5 = ttl_detail::null_type
    >
    typename enable_if<models<Concept, Arg0, Arg1, Arg2, Arg3, Arg4, Arg5>::value>::type models_refines();
}

#endif