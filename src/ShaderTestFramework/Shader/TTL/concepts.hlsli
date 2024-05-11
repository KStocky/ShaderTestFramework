
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

    struct invocable_functor
    {
        template<
            typename F
            >
        __decltype(
            declval<F>()()
        ) requires();

        template<
            typename F,
            typename Arg0
            >
        __decltype(
            declval<F>()(declval<Arg0>())
        ) requires();

        template<
            typename F,
            typename Arg0, typename Arg1
            >
        __decltype(
            declval<F>()(declval<Arg0>(), declval<Arg1>())
        ) requires();

        template<
            typename F,
            typename Arg0, typename Arg1, typename Arg2
            >
        __decltype(
            declval<F>()(declval<Arg0>(), declval<Arg1>(), declval<Arg2>())
        ) requires();

        template<
            typename F,
            typename Arg0, typename Arg1, typename Arg2, typename Arg3
            >
        __decltype(
            declval<F>()(declval<Arg0>(), declval<Arg1>(), declval<Arg2>(), declval<Arg3>())
        ) requires();

        template<
            typename F,
            typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4
            >
        __decltype(
            declval<F>()(declval<Arg0>(), declval<Arg1>(), declval<Arg2>(), declval<Arg3>(), declval<Arg4>())
        ) requires();
    };

    struct invocable
    {
        template<
            typename F
            >
        __decltype(
            models_if<
                models<invocable_functor, F>::value ||
                is_invocable_function<F>::value
            >()
        ) requires();

        template<
            typename F,
            typename Arg0
            >
        __decltype(
            models_if<
                models<invocable_functor, F, Arg0>::value ||
                is_invocable_function<F, Arg0>::value
            >()
        ) requires();
        
        template<
            typename F,
            typename Arg0, typename Arg1
            >
        __decltype(
            models_if<
                models<invocable_functor, F, Arg0, Arg1>::value ||
                is_invocable_function<F, Arg0, Arg1>::value
            >()
        ) requires();

        template<
            typename F,
            typename Arg0, typename Arg1, typename Arg2
            >
        __decltype(
            models_if<
                models<invocable_functor, F, Arg0, Arg1, Arg2>::value ||
                is_invocable_function<F, Arg0, Arg1, Arg2>::value
            >()
        ) requires();

        template<
            typename F,
            typename Arg0, typename Arg1, typename Arg2, typename Arg3
            >
        __decltype(
            models_if<
                models<invocable_functor, F, Arg0, Arg1, Arg2, Arg3>::value ||
                is_invocable_function<F, Arg0, Arg1, Arg2, Arg3>::value
            >()
        ) requires();

        template<
            typename F,
            typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4
            >
        __decltype(
            models_if<
                models<invocable_functor, F, Arg0, Arg1, Arg2, Arg3, Arg4>::value ||
                is_invocable_function<F, Arg0, Arg1, Arg2, Arg3, Arg4>::value
            >()
        ) requires();
        
    };
}

#endif
