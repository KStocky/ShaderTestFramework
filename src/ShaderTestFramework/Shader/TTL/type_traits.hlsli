#pragma once

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
    
    template<typename T, typename U>
    struct is_same : false_type
    {
    };
    
    template<typename T>
    struct is_same<T, T> : true_type 
    {
    };
    
    template<bool InCond, typename T = void>
    struct enable_if
    {
    };
    
    template<typename T>
    struct enable_if<true, T>
    {
        using type = T;  
    };

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

    template<typename T, typename = void>
    struct align_of : integral_constant<uint, ((uint)sizeof(T) < 8u) ? (uint)sizeof(T) : 8 >{};

    template<typename T>
    struct align_of<T, typename enable_if<fundamental_type_traits<T>::is_fundamental>::type> 
        : integral_constant<uint, sizeof(typename fundamental_type_traits<T>::base_type)>{};

    // Depends on https://github.com/microsoft/DirectXShaderCompiler/issues/5553
    template<typename T, typename = void>
    struct is_enum
    {
        static const bool value = true;
    };

    template<typename T>
    struct is_enum<T, typename enable_if<sizeof(T) != 0>::type>
    {
        static const bool value = false;
    };
}