#ifndef STF_ASSERTIONSV1_STRINGS_HEADER
#define STF_ASSERTIONSV1_STRINGS_HEADER

#include "/Test/STF/AssertionsV1/FrameworkResources.hlsli"

#include "/Test/TTL/byte_writer.hlsli"
#include "/Test/TTL/memory.hlsli"
#include "/Test/TTL/string.hlsli"

namespace stf
{
    namespace AssertionsV1
    {
        namespace detail
        {
            template<uint N>
            uint2 AddStringData(ttl::string<N> In)
            {
                const uint size = ttl::aligned_offset(In.Size, 4u);
                uint offset = 0;
                GetAllocationBuffer().InterlockedAdd(StringDataSizeIndex, size, offset);

                const uint startAddress = Strings.BeginData() + offset;
                uint address = startAddress;
                if (offset + size < Strings.SizeInBytesOfData())
                {
                    RWByteAddressBuffer buff = GetTestDataBuffer();
                    ttl::write_bytes(buff, address, In);

                    return uint2(startAddress, size);
                }

                return uint2(0, 0);
            }

            void AddStringMetaInfo(const uint InMetaIndex, const uint2 InAddressAndSize)
            {
                RWByteAddressBuffer buffer = GetTestDataBuffer();
                const uint metaAddress = InMetaIndex * sizeof(StringMetaData) + Strings.BeginMeta();
                buffer.Store2(metaAddress, InAddressAndSize);
            }
        }
    }
}

#endif