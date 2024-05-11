#include "/Test/TTL/static_assert.hlsli"
#include "/Test/TTL/type_traits.hlsli"

struct A
{
    using type = void;
};

struct B
{

};

STATIC_ASSERT((ttl::is_same<void, ttl::void_t<> >::value));
STATIC_ASSERT((ttl::is_same<void, ttl::void_t<A> >::value));
STATIC_ASSERT((ttl::is_same<void, ttl::void_t<A, A> >::value));
STATIC_ASSERT((ttl::is_same<void, ttl::void_t<A, A, A> >::value));
STATIC_ASSERT((ttl::is_same<void, ttl::void_t<A, A, A, A> >::value));
STATIC_ASSERT((ttl::is_same<void, ttl::void_t<A, A, A, A, A> >::value));
STATIC_ASSERT((ttl::is_same<void, ttl::void_t<A, A, A, A, A, A> >::value));
STATIC_ASSERT((ttl::is_same<void, ttl::void_t<A, A, A, A, A, A, A> >::value));
STATIC_ASSERT((ttl::is_same<void, ttl::void_t<A, A, A, A, A, A, A, A> >::value));
STATIC_ASSERT((ttl::is_same<void, ttl::void_t<A, A, A, A, A, A, A, A, A> >::value));
STATIC_ASSERT((ttl::is_same<void, ttl::void_t<A, A, A, A, A, A, A, A, A, A> >::value));

template<
    typename T0 = A, typename T1 = A , typename T2 = A, typename T3 = A, typename T4 = A,
    typename T5 = A, typename T6 = A , typename T7 = A, typename T8 = A, typename T9 = A, typename = void
    >
struct VoidTDetector
{
    static const bool value = false;
};


template<
    typename T0, typename T1, typename T2, typename T3, typename T4,
    typename T5, typename T6, typename T7, typename T8, typename T9
    >
struct VoidTDetector<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
    ttl::void_t<typename T0::type, typename T1::type, typename T2::type, typename T3::type, typename T4::type, typename T5::type, typename T6::type, typename T7::type, typename T8::type, typename T9::type> >
{
    static const bool value = true;
};

STATIC_ASSERT((VoidTDetector<A>::value));
STATIC_ASSERT((!VoidTDetector<B>::value));
STATIC_ASSERT((!VoidTDetector<A, B>::value));
STATIC_ASSERT((!VoidTDetector<B, A>::value));
STATIC_ASSERT((!VoidTDetector<A, A, B>::value));
STATIC_ASSERT((!VoidTDetector<A, A, A, B>::value));
STATIC_ASSERT((!VoidTDetector<A, A, A, A, B>::value));
STATIC_ASSERT((!VoidTDetector<A, A, A, A, A, B>::value));
STATIC_ASSERT((!VoidTDetector<A, A, A, A, A, A, B>::value));
STATIC_ASSERT((!VoidTDetector<A, A, A, A, A, A, A, B>::value));
STATIC_ASSERT((!VoidTDetector<A, A, A, A, A, A, A, A, B>::value));
STATIC_ASSERT((!VoidTDetector<A, A, A, A, A, A, A, A, A, B>::value));
