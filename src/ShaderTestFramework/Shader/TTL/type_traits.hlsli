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
    struct is_fundamental : false_type{};

    template<> struct is_fundamental<bool> : true_type{};
    template<> struct is_fundamental<int> : true_type{};
    template<> struct is_fundamental<int2> : true_type{};
    template<> struct is_fundamental<int3> : true_type{};
    template<> struct is_fundamental<int4> : true_type{};
    template<> struct is_fundamental<uint> : true_type{};
    template<> struct is_fundamental<uint2> : true_type{};
    template<> struct is_fundamental<uint3> : true_type{};
    template<> struct is_fundamental<uint4> : true_type{};
    template<> struct is_fundamental<float> : true_type{};
    template<> struct is_fundamental<float2> : true_type{};
    template<> struct is_fundamental<float3> : true_type{};
    template<> struct is_fundamental<float4> : true_type{};

    template<typename T>
    struct fundamental_type_traits;

    template<typename T, uint InRank>
    struct fundamental_type_traits_base
    {
        using base_type = T;
        static const uint rank = InRank;
    };

    template<> struct fundamental_type_traits<bool> : fundamental_type_traits_base<bool, 1>{};

    template<> struct fundamental_type_traits<uint> : fundamental_type_traits_base<uint, 1>{};
    template<> struct fundamental_type_traits<uint2> : fundamental_type_traits_base<uint, 2>{};
    template<> struct fundamental_type_traits<uint3> : fundamental_type_traits_base<uint, 3>{};
    template<> struct fundamental_type_traits<uint4> : fundamental_type_traits_base<uint, 4>{};

    template<> struct fundamental_type_traits<int> : fundamental_type_traits_base<int, 1>{};
    template<> struct fundamental_type_traits<int2> : fundamental_type_traits_base<int, 2>{};
    template<> struct fundamental_type_traits<int3> : fundamental_type_traits_base<int, 3>{};
    template<> struct fundamental_type_traits<int4> : fundamental_type_traits_base<int, 4>{};

    template<> struct fundamental_type_traits<float> : fundamental_type_traits_base<float, 1>{};
    template<> struct fundamental_type_traits<float2> : fundamental_type_traits_base<float, 2>{};
    template<> struct fundamental_type_traits<float3> : fundamental_type_traits_base<float, 3>{};
    template<> struct fundamental_type_traits<float4> : fundamental_type_traits_base<float, 4>{};
}