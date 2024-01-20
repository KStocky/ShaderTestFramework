#pragma once

#include "/Test/TTL/container_wrapper.hlsli"
#include "/Test/TTL/type_traits.hlsli"

namespace ttl
{
    template<typename T, typename = void>
    struct byte_writer
    {
        static const bool has_writer = false;
    };

    template<typename T>
    struct byte_writer<T, typename enable_if <fundamental_type_traits<T>::is_fundamental>::type>
    {
        static const bool has_writer = true;
        static const bool is_bool_writer = is_same<typename fundamental_type_traits<T>::base_type, bool>::value;

        static uint bytes_required(T)
        {
            return sizeof(T);
        }

        template<typename ContainerType, uint InRank, bool ForBools = false>
        struct WriteEnabler
        {
            static const bool cond_for_bools = 
                is_same<typename container_traits<ContainerType>::element_type, uint>::value && 
                ForBools;
            
            static const bool cond_for_non_bools = 
                is_same<typename container_traits<ContainerType>::element_type, uint>::value && 
                fundamental_type_traits<T>::dim0 == InRank;
            static const bool value = is_bool_writer ? cond_for_bools : cond_for_non_bools;
        };

        template<typename U>
        static typename enable_if<WriteEnabler<U, 1>::value>::type write(inout container_wrapper<U> InContainer, const uint InIndex, const T In)
        {
            InContainer.store(InIndex, asuint(In));
        }

        template<typename U>
        static typename enable_if<WriteEnabler<U, 2>::value>::type write(inout container_wrapper<U> InContainer, const uint InIndex, const T In)
        {
            InContainer.store(InIndex, asuint(In.x), asuint(In.y));
        }

        template<typename U>
        static typename enable_if<WriteEnabler<U, 3>::value>::type write(inout container_wrapper<U> InContainer, const uint InIndex, const T In)
        {
            InContainer.store(InIndex, asuint(In.x), asuint(In.y), asuint(In.z));
        }

        template<typename U>
        static typename enable_if<WriteEnabler<U, 4>::value>::type write(inout container_wrapper<U> InContainer, const uint InIndex, const T In)
        {
            InContainer.store(InIndex, asuint(In.x), asuint(In.y), asuint(In.z), asuint(In.w));
        }
        
        template<typename U>
        static typename enable_if<WriteEnabler<U, 0, true>::value>::type write(inout container_wrapper<U> InContainer, const uint InIndex, const T In)
        {
            byte_writer<uint>::write(InContainer, InIndex, In ? 1u : 0u);
        }
    };

    template<typename T>
    typename enable_if<byte_writer<T>::has_writer, uint>::type bytes_required(T In)
    {
        return byte_writer<T>::bytes_required(In);
    }

    template<typename T>
    typename enable_if<!byte_writer<T>::has_writer, uint>::type bytes_required(T In)
    {
        return sizeof(T);
    }

    template<typename T, typename U>
    typename enable_if<
        byte_writer<T>::has_writer &&
        container_traits<U>::is_container &&
        container_traits<U>::is_writable &&
        is_same<uint, typename container_traits<U>::element_type>::value
    >::type 
    write_bytes(inout container_wrapper<U> InContainer, const uint InIndex, const T In)
    {
        byte_writer<T>::write(InContainer, InIndex, In);
    }

    template<typename T, typename U>
    typename enable_if<
        byte_writer<T>::has_writer &&
        container_traits<U>::is_container &&
        container_traits<U>::is_writable &&
        is_same<uint, typename container_traits<U>::element_type>::value
    >::type 
    write_bytes(inout U InContainer, const uint InIndex, const T In)
    {
        container_wrapper<U> buff;
        buff.Data = InContainer;
        write_bytes(buff, InIndex, In);
        InContainer = buff.Data;
    }
}