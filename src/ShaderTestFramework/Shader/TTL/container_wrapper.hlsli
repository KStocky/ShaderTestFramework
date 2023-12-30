#pragma once

#include "/Test/TTL/type_traits.hlsli"

namespace ttl
{
    template<typename ContainerType, typename = void>
    struct container_wrapper;
    
    template<typename T>
    struct container_wrapper<T, typename enable_if<container_traits<T>::is_container>::type>
    {
        using underlying_type = T;
        using element_type = typename container_traits<underlying_type>::element_type;
        static const bool writable = container_traits<underlying_type>::is_writable;

        underlying_type Data;

        template<typename U = T>
        typename enable_if<is_same<U, T>::value && array_traits<U>::is_array, uint>::type size()
        {
            return array_traits<U>::size;
        }

        template<typename U = T>
        typename enable_if<is_same<U, T>::value && !array_traits<U>::is_array, uint>::type size()
        {
            uint ret = 0;
            Data.GetDimensions(ret);
            return ret;
        }

        uint size_in_bytes()
        {
            return sizeof(element_type) * size();
        }

        template<typename U = T>
        typename enable_if<is_same<U, T>::value && !array_traits<U>::is_array, uint>::type load(const uint InIndex)
        {
            return Data.Load(InIndex);
        }

        template<typename U = T>
        typename enable_if<is_same<U, T>::value && array_traits<U>::is_array, uint>::type load(const uint InIndex)
        {
            return Data[InIndex];
        }

        template<typename U>
        struct BufferEnabler
        {
            static const bool value = writable && is_same<element_type, U>::value && !container_traits<underlying_type>::is_byte_address;
        };

        template<typename U>
        struct ByteAddressEnabler
        {
            static const bool value = writable && is_same<element_type, U>::value && container_traits<underlying_type>::is_byte_address;
        };

        template<typename U>
        typename enable_if<BufferEnabler<U>::value>::type store(const uint InIndex, const U InItem)
        {
            Data[InIndex] = InItem;
        }

        template<typename U>
        typename enable_if<BufferEnabler<U>::value>::type store(const uint InIndex, const U InItem, const U InItem2)
        {
            Data[InIndex] = InItem;
            Data[InIndex + 1] = InItem2;
        }

        template<typename U>
        typename enable_if<BufferEnabler<U>::value>::type store(const uint InIndex, const U InItem, const U InItem2, const U InItem3)
        {
            Data[InIndex] = InItem;
            Data[InIndex + 1] = InItem2;
            Data[InIndex + 2] = InItem3;
        }

        template<typename U>
        typename enable_if<BufferEnabler<U>::value>::type store(const uint InIndex, const U InItem, const U InItem2, const U InItem3, const U InItem4)
        {
            Data[InIndex] = InItem;
            Data[InIndex + 1] = InItem2;
            Data[InIndex + 2] = InItem3;
            Data[InIndex + 3] = InItem4;
        }

        template<typename U>
        typename enable_if<ByteAddressEnabler<U>::value>::type store(const uint InIndex, const U InItem)
        {
            Data.Store(InIndex, InItem);
        }

        template<typename U>
        typename enable_if<ByteAddressEnabler<U>::value>::type store(const uint InIndex, const U InItem, const U InItem2)
        {
            Data.Store(InIndex, uint2(InItem, InItem2));
        }

        template<typename U>
        typename enable_if<ByteAddressEnabler<U>::value>::type store(const uint InIndex, const U InItem, const U InItem2, const U InItem3)
        {
            Data.Store(InIndex, uint3(InItem, InItem2, InItem3));
        }

        template<typename U>
        typename enable_if<ByteAddressEnabler<U>::value>::type store(const uint InIndex, const U InItem, const U InItem2, const U InItem3, const U InItem4)
        {
            Data.Store(InIndex, uint4(InItem, InItem2, InItem3, InItem4));
        }
    };
}