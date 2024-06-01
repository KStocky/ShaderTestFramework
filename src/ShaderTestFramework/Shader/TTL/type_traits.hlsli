
#ifndef TTL_TYPE_TRAITS_HEADER
#define TTL_TYPE_TRAITS_HEADER

namespace ttl
{
    template<typename T, T v>
    struct integral_constant
    {
        static const T value = v;
        using value_type = T;
        using type = integral_constant;
    };
    
    using true_type = integral_constant<bool, true>;
    using false_type = integral_constant<bool, false>;
}

namespace ttl
{
    template<bool InCond, typename T1, typename T2>
    struct conditional
    {
        using type = T1;
    };

    template<typename T1, typename T2>
    struct conditional<false, T1, T2>
    {
        using type = T2;
    };

    template<bool InCond, typename T1, typename T2>
    using conditional_t = typename conditional<InCond, T1, T2>::type;
}

namespace ttl
{
    template<typename T, T Ignore, T Pick>
    struct pick_right : integral_constant<T, Pick>{};
}

namespace ttl
{
    template<typename T, typename U>
    struct is_same : false_type
    {
    };
    
    template<typename T>
    struct is_same<T, T> : true_type 
    {
    };

    template<typename T, typename U>
    static const bool is_same_v = is_same<T, U>::value;
}

namespace ttl
{
    template<bool InCond, typename T = void>
    struct enable_if
    {
    };
    
    template<typename T>
    struct enable_if<true, T>
    {
        using type = T;  
    };

    template<bool InCond, typename T = void>
    using enable_if_t = typename enable_if<InCond, T>::type;
}

namespace ttl
{
    template<typename T>
    struct array_traits
    {
        static const bool is_array = false;
        static const uint size = 0;
        using element_type = void;
    };

    template<typename T, uint Size>
    struct array_traits<T[Size]>
    {
        static const bool is_array = true;
        static const uint size = Size;
        using element_type = T;
    };
}

namespace ttl
{
    template<typename T>
    struct container_traits
    {
        static const bool is_container = false;
        static const bool is_writable = false;
        static const bool is_byte_address = false;
        static const bool is_structured = false;
        static const bool is_resource = false;
        using element_type = void;
        using type = void;
    };

    template<typename T, uint Size>
    struct container_traits<T[Size]>
    {
        static const bool is_container = true;
        static const bool is_writable = true;
        static const bool is_byte_address = false;
        static const bool is_structured = false;
        static const bool is_resource = false;
        using element_type = T;
        using type = T[Size];
    };

    template<typename T>
    struct container_traits<Buffer<T> >
    {
        static const bool is_container = true;
        static const bool is_writable = false;
        static const bool is_byte_address = false;
        static const bool is_structured = false;
        static const bool is_resource = true;
        using element_type = T;
        using type = Buffer<T>;
    };

    template<typename T>
    struct container_traits<RWBuffer<T> >
    {
        static const bool is_container = true;
        static const bool is_writable = true;
        static const bool is_byte_address = false;
        static const bool is_structured = false;
        static const bool is_resource = true;
        using element_type = T;
        using type = RWBuffer<T>;
    };

    template<typename T>
    struct container_traits<StructuredBuffer<T> >
    {
        static const bool is_container = true;
        static const bool is_writable = false;
        static const bool is_byte_address = false;
        static const bool is_structured = true;
        static const bool is_resource = true;
        using element_type = T;
        using type = StructuredBuffer<T>;
    };

    template<typename T>
    struct container_traits<RWStructuredBuffer<T> >
    {
        static const bool is_container = true;
        static const bool is_writable = true;
        static const bool is_byte_address = false;
        static const bool is_structured = true;
        static const bool is_resource = true;
        using element_type = T;
        using type = RWStructuredBuffer<T>;
    };

    template<>
    struct container_traits<ByteAddressBuffer>
    {
        static const bool is_container = true;
        static const bool is_writable = false;
        static const bool is_byte_address = true;
        static const bool is_structured = false;
        static const bool is_resource = true;
        using element_type = uint;
        using type = ByteAddressBuffer;
    };

    template<>
    struct container_traits<RWByteAddressBuffer>
    {
        static const bool is_container = true;
        static const bool is_writable = true;
        static const bool is_byte_address = true;
        static const bool is_structured = false;
        static const bool is_resource = true;
        using element_type = uint;
        using type = RWByteAddressBuffer;
    };
}

namespace ttl
{
    template<typename T>
    struct fundamental_type_traits
    {
        using base_type = void;
        static const bool is_fundamental = false;
        static const uint dim0 = 0;
        static const uint dim1 = 0;
    };

    template<typename T, uint InDim0, uint InDim1>
    struct fundamental_type_traits_base
    {
        using base_type = T;
        static const bool is_fundamental = true;
        static const uint dim0 = InDim0;
        static const uint dim1 = InDim1;
    };

    template<> struct fundamental_type_traits<bool> : fundamental_type_traits_base<bool, 1, 1>{};
    template<> struct fundamental_type_traits<int16_t> : fundamental_type_traits_base<int16_t, 1, 1>{};
    template<> struct fundamental_type_traits<int32_t> : fundamental_type_traits_base<int32_t, 1, 1>{};
    template<> struct fundamental_type_traits<int64_t> : fundamental_type_traits_base<int64_t, 1, 1>{};

    template<> struct fundamental_type_traits<uint16_t> : fundamental_type_traits_base<uint16_t, 1, 1>{};
    template<> struct fundamental_type_traits<uint32_t> : fundamental_type_traits_base<uint32_t, 1, 1>{};
    template<> struct fundamental_type_traits<uint64_t> : fundamental_type_traits_base<uint64_t, 1, 1>{};

    template<> struct fundamental_type_traits<float16_t> : fundamental_type_traits_base<float16_t, 1, 1>{};
    template<> struct fundamental_type_traits<float32_t> : fundamental_type_traits_base<float32_t, 1, 1>{};
    template<> struct fundamental_type_traits<float64_t> : fundamental_type_traits_base<float64_t, 1, 1>{};

    template<typename T, uint InDim> struct fundamental_type_traits<vector<T, InDim> > : fundamental_type_traits_base<T, InDim, 1>{};
    template<typename T, uint InDim0, uint InDim1> struct fundamental_type_traits<matrix<T, InDim0, InDim1> > : fundamental_type_traits_base<T, InDim0, InDim1>{};
}

namespace ttl
{
    // Depends on https://github.com/microsoft/DirectXShaderCompiler/issues/5553
    template<typename T, typename = void>
    struct is_or_has_enum : true_type{};

    template<typename T>
    struct is_or_has_enum<T, typename enable_if<sizeof(T) != 0>::type> : false_type{};

    template<typename T>
    static const bool is_or_has_enum_v = is_or_has_enum<T>::value;
}

namespace ttl_detail
{
    template<typename T>
    struct wrapper
    {
        T dummy;
    };

    template<typename T>
    struct size_of_helper : wrapper<T>{};
}

namespace ttl
{
    template<typename T, typename = void>
    struct size_of : integral_constant<uint, sizeof(ttl_detail::size_of_helper<T>)>{};

    template<typename T>
    static const uint size_of_v = size_of<T>::value;
}

namespace ttl_detail
{
    template<typename T>
    struct offset_lowest_align_wrapper
    {
        T first;
        int16_t second;
    };
}

namespace ttl
{
    template<typename T>
    struct align_of : integral_constant<uint, size_of<ttl_detail::offset_lowest_align_wrapper<T> >::value - size_of<T>::value>{};

    template<typename T>
    static const uint align_of_v = align_of<T>::value;
}

namespace ttl
{
    template<typename T>
    T declval();
}

namespace ttl
{
    template<
        typename T0 = void, typename T1 = void , typename T2 = void, typename T3 = void, typename T4 = void,
        typename T5 = void, typename T6 = void , typename T7 = void, typename T8 = void, typename T9 = void
        >
    using void_t = void;
}

namespace ttl_detail
{
    template<typename T>
    void test_conv(T);

    template<typename Base, typename Derived, typename = void>
    struct is_base_of_helper : ttl::false_type {};

    template<typename Base, typename Derived>
    struct is_base_of_helper<Base, Derived, __decltype(test_conv<Base>(ttl::declval<Derived>())) > : ttl::true_type{};
}

namespace ttl
{
    template<typename Base, typename Derived>
    struct is_base_of : 
        integral_constant<
            bool,
            ttl_detail::is_base_of_helper<Base, Derived>::value &&
            !ttl::fundamental_type_traits<Base>::is_fundamental &&
            !ttl::fundamental_type_traits<Derived>::is_fundamental
        >{};

    template<typename Base, typename Derived>
    static const bool is_base_of_v = is_base_of<Base, Derived>::value;
}

namespace ttl_detail
{
    template<typename T, uint N>
    void is_array_helper(T In[N]);
}

namespace ttl
{
    template<typename T, typename = void>
    struct is_array : false_type {};

    template<typename T>
    struct is_array<T, typename enable_if<array_traits<T>::is_array>::type> : true_type {};

    template<typename T>
    struct is_array<T, void_t<__decltype(ttl_detail::is_array_helper(declval<T>()))> > : true_type {};

    template<typename T>
    static const bool is_array_v = is_array<T>::value;
}

namespace ttl
{
    template<typename T, uint N>
    integral_constant<uint, N> array_len(T In[N]);

    template<typename T>
    typename enable_if<!is_array<T>::value, integral_constant<uint, 0> >::type array_len(T In);
}

namespace ttl
{
    template<typename T> struct is_function : false_type {};
    template<typename RetType> struct is_function<RetType()> : true_type {};
    template<typename RetType, typename Arg0> struct is_function<RetType(Arg0)> : true_type {};
    template<typename RetType, typename Arg0, typename Arg1> struct is_function<RetType(Arg0, Arg1)> : true_type {};
    template<typename RetType, typename Arg0, typename Arg1, typename Arg2> struct is_function<RetType(Arg0, Arg1, Arg2)> : true_type {};
    template<typename RetType, typename Arg0, typename Arg1, typename Arg2, typename Arg3> struct is_function<RetType(Arg0, Arg1, Arg2, Arg3)> : true_type {};
    template<typename RetType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4> struct is_function<RetType(Arg0, Arg1, Arg2, Arg3, Arg4)> : true_type {};
    template<typename RetType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5> struct is_function<RetType(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5)> : true_type {};

    template<typename T>
    static const bool is_function_v = is_function<T>::value;
}

namespace ttl
{
    template<typename T, typename Arg0 = void, typename Arg1 = void, typename Arg2 = void, typename Arg3 = void, typename Arg4 = void, typename Arg5 = void> 
    struct is_invocable_function : false_type {};

    template<typename RetType> 
    struct is_invocable_function<RetType()> : true_type {};

    template<typename RetType, typename Arg0> 
    struct is_invocable_function<RetType(Arg0), Arg0> : true_type {};

    template<typename RetType, typename Arg0, typename Arg1> 
    struct is_invocable_function<RetType(Arg0, Arg1), Arg0, Arg1> : true_type {};

    template<typename RetType, typename Arg0, typename Arg1, typename Arg2> 
    struct is_invocable_function<RetType(Arg0, Arg1, Arg2), Arg0, Arg1, Arg2> : true_type {};

    template<typename RetType, typename Arg0, typename Arg1, typename Arg2, typename Arg3> 
    struct is_invocable_function<RetType(Arg0, Arg1, Arg2, Arg3), Arg0, Arg1, Arg2, Arg3> : true_type {};

    template<typename RetType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4> 
    struct is_invocable_function<RetType(Arg0, Arg1, Arg2, Arg3, Arg4), Arg0, Arg1, Arg2, Arg3, Arg4> : true_type {};

    template<typename RetType, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5> 
    struct is_invocable_function<RetType(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5), Arg0, Arg1, Arg2, Arg3, Arg4, Arg5> : true_type {};

    template<bool InCond, typename T1, typename T2>
    using conditional_t = typename conditional<InCond, T1, T2>::type;

    template<typename T, typename Arg0 = void, typename Arg1 = void, typename Arg2 = void, typename Arg3 = void, typename Arg4 = void, typename Arg5 = void> 
    static const bool is_invocable_function_v = 
        conditional_t<is_same_v<Arg0, void>, is_invocable_function<T>,
        conditional_t<is_same_v<Arg1, void>, is_invocable_function<T, Arg0>, 
        conditional_t<is_same_v<Arg2, void>, is_invocable_function<T, Arg0, Arg1>,
        conditional_t<is_same_v<Arg3, void>, is_invocable_function<T, Arg0, Arg1, Arg2>,
        conditional_t<is_same_v<Arg4, void>, is_invocable_function<T, Arg0, Arg1, Arg2, Arg3>,
        conditional_t<is_same_v<Arg4, void>, is_invocable_function<T, Arg0, Arg1, Arg2, Arg3, Arg4>,
                                             is_invocable_function<T, Arg0, Arg1, Arg2, Arg3, Arg4, Arg5>
        > > > > > >::value;
}

namespace ttl
{
    template<typename T>
    struct always_false : false_type{};

    template<typename T>
    static const bool always_false_v = always_false<T>::value;
}

#endif