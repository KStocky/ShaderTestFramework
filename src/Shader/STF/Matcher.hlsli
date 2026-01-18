#ifndef STF_MATCHER_HEADER
#define STF_MATCHER_HEADER

#include "/Test/TTL/concepts.hlsli"
#include "/Test/TTL/memory.hlsli"
#include "/Test/TTL/models.hlsli"
#include "/Test/TTL/type_traits.hlsli"

namespace stf
{
    namespace detail
    {
        template<typename T, typename = void>
        struct has_bool_base_impl : ttl::false_type{};

        template<typename T>
        struct has_bool_base_impl<T, ttl::enable_if_t<ttl::fundamental_type_traits<T>::is_fundamental> > : ttl::is_same<bool, typename ttl::fundamental_type_traits<T>::base_type>{};

        template<typename T>
        using has_bool_base = has_bool_base_impl<T>;

        template<typename T>
        static const bool has_bool_base_v = has_bool_base<T>::value;
    }

    struct matcher_type
    {
        template<typename T, typename U>
        __decltype(
            ttl::models_if_same<bool, __decltype(ttl::declval<T>().Matches(ttl::declval<U>()))>(),
            ttl::declval<T>().Value
        ) requires();
    };

    struct EqualComparableWith
    {
        template<typename T, typename U>
        __decltype(
            ttl::models_if_pred<detail::has_bool_base, __decltype(ttl::declval<T>() == ttl::declval<U>())>()
        ) requires();
    };

    struct LessThanComparableWith
    {
        template<typename T, typename U>
        __decltype(
            ttl::models_if_pred<detail::has_bool_base, __decltype(ttl::declval<T>() < ttl::declval<U>())>()
        ) requires();
    };

    struct LessThanEqualComparableWith
    {
        template<typename T, typename U>
        __decltype(
            ttl::models_if_pred<detail::has_bool_base, __decltype(ttl::declval<T>() <= ttl::declval<U>())>()
        ) requires();
    };

    struct GreaterThanEqualComparableWith
    {
        template<typename T, typename U>
        __decltype(
            ttl::models_if_pred<detail::has_bool_base, __decltype(ttl::declval<T>() >= ttl::declval<U>())>()
        ) requires();
    };

    struct GreaterThanComparableWith
    {
        template<typename T, typename U>
        __decltype(
            ttl::models_if_pred<detail::has_bool_base, __decltype(ttl::declval<T>() > ttl::declval<U>())>()
        ) requires();
    };

    template<typename T>
    struct EqualsMatcher
    {
        template<typename U>
        ttl::models_t<bool, EqualComparableWith, U, T> Matches(U InLHS)
        {
            return all(InLHS == Value);
        }

        T Value;
    };

    template<typename T>
    EqualsMatcher<T> Equals(T InVal)
    {
        EqualsMatcher<T> ret = ttl::zero<EqualsMatcher<T> >();
        ret.Value = InVal;
        return ret;
    }

    template<typename T>
    struct LessThanMatcher
    {
        template<typename U>
        ttl::models_t<bool, LessThanComparableWith, U, T> Matches(U InLHS)
        {
            return all(InLHS < Value);
        }

        T Value;
    };

    template<typename T>
    LessThanMatcher<T> LessThan(T InVal)
    {
        LessThanMatcher<T> ret = ttl::zero<LessThanMatcher<T> >();
        ret.Value = InVal;
        return ret;
    }

    template<typename T>
    struct LessThanEqualMatcher
    {
        template<typename U>
        ttl::models_t<bool, LessThanEqualComparableWith, U, T> Matches(U InLHS)
        {
            return all(InLHS <= Value);
        }

        T Value;
    };

    template<typename T>
    LessThanEqualMatcher<T> LessThanEqual(T InVal)
    {
        LessThanEqualMatcher<T> ret = ttl::zero<LessThanEqualMatcher<T> >();
        ret.Value = InVal;
        return ret;
    }

    template<typename T>
    struct GreaterThanEqualMatcher
    {
        template<typename U>
        ttl::models_t<bool, GreaterThanEqualComparableWith, U, T> Matches(U InLHS)
        {
            return all(InLHS >= Value);
        }

        T Value;
    };

    template<typename T>
    GreaterThanEqualMatcher<T> GreaterThanEqual(T InVal)
    {
        GreaterThanEqualMatcher<T> ret = ttl::zero<GreaterThanEqualMatcher<T> >();
        ret.Value = InVal;
        return ret;
    }

    template<typename T>
    struct GreaterThanMatcher
    {
        template<typename U>
        ttl::models_t<bool, GreaterThanComparableWith, U, T> Matches(U InLHS)
        {
            return all(InLHS > Value);
        }

        T Value;
    };

    template<typename T>
    GreaterThanMatcher<T> GreaterThan(T InVal)
    {
        GreaterThanMatcher<T> ret = ttl::zero<GreaterThanMatcher<T> >();
        ret.Value = InVal;
        return ret;
    }
}

#endif