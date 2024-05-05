#ifndef TTL_BYTE_WRITER_HEADER
#define TTL_BYTE_WRITER_HEADER

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
    typename enable_if<byte_writer<T>::has_writer, uint>::type bytes_required(T In)
    {
        return byte_writer<T>::bytes_required(In);
    }

    template<typename T>
    typename enable_if<!byte_writer<T>::has_writer, uint>::type bytes_required(T)
    {
        return size_of<T>::value;
    }

    template<typename T>
    typename enable_if<byte_writer<T>::has_writer, uint>::type alignment_required(T In)
    {
        return byte_writer<T>::alignment_required(In);
    }

    template<typename T>
    typename enable_if<!byte_writer<T>::has_writer, uint>::type alignment_required(T)
    {
        return align_of<T>::value;
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
        !byte_writer<T>::has_writer &&
        container_traits<U>::is_container &&
        container_traits<U>::is_writable &&
        container_traits<U>::is_byte_address
    >::type 
    write_bytes(inout container_wrapper<U> InContainer, const uint InIndex, const T In)
    {
        InContainer.store(InIndex, In);
    }

    template<typename T, typename U>
    typename enable_if<
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

#endif
