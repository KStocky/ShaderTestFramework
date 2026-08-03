#ifndef STF_ASSERTIONSV1_ASSERTIONS_HEADER
#define STF_ASSERTIONSV1_ASSERTIONS_HEADER

#include "/Test/STF/AssertionsV1/FrameworkResources.hlsli"
#include "/Test/STF/ByteReaderTraits.hlsli"
#include "/Test/STF/SectionManagement.hlsli"

#include "/Test/TTL/byte_writer.hlsli"
#include "/Test/TTL/caster.hlsli"
#include "/Test/TTL/macro.hlsli"
#include "/Test/TTL/memory.hlsli"


namespace stf
{
    namespace AssertionsV1
    {
        namespace detail
        {
            void Success()
            {
                uint successIndex;
                GetAllocationBuffer().InterlockedAdd(NumSuccessIndex, 1, successIndex);
            }
        
            uint AddAssert()
            {
                uint assertIndex;
                GetAllocationBuffer().InterlockedAdd(NumFailsIndex, 1, assertIndex);
                return assertIndex;
            }

            void AddAssertMetaInfo(const uint InMetaIndex, const int InId, const uint InReaderAndTypeId, const uint2 InAddressAndSize)
            {
                RWByteAddressBuffer buffer = GetTestDataBuffer();
                const uint metaAddress = InMetaIndex * sizeof(HLSLAssertMetaData);
                buffer.Store4(metaAddress, uint4((uint)InId, stf::detail::Scratch.ThreadID));
                buffer.Store4(metaAddress + 16, uint4((uint)stf::detail::Scratch.GetSectionID(), InReaderAndTypeId, InAddressAndSize));
            }

            
            template<typename T>
            uint2 AddAssertData(T In1, T In2)
            {
                const uint size1 = ttl::bytes_required(In1);
                const uint size2 = ttl::bytes_required(In2);
                const uint align1 = ttl::alignment_required(In1);
                const uint align2 = ttl::alignment_required(In2);
                const uint sizeAndAlign1 = (size1 << 16) | align1;
                const uint sizeAndAlign2 = (size2 << 16) | align2;

                const uint alignedSize1 = ttl::aligned_offset(size1 + 4, align1);
                const uint alignedSize2 = ttl::aligned_offset(size2 + 4, align2);
                const uint size = ttl::aligned_offset(alignedSize1 + alignedSize2, 8);
                uint offset = 0;
                GetAllocationBuffer().InterlockedAdd(AssertDataSizeIndex, size, offset);

                const uint startAddress = Asserts.BeginData() + offset;
                uint address = startAddress;
                if (offset + size < Asserts.SizeInBytesOfData())
                {
                    RWByteAddressBuffer buff = GetTestDataBuffer();
                    ttl::write_bytes(buff, address, sizeAndAlign1);
                    address = ttl::aligned_offset(address + 4, align1);
                    ttl::write_bytes(buff, address, In1);
                    address = ttl::aligned_offset(address + size1, 4);
                    ttl::write_bytes(buff, address, sizeAndAlign2);
                    address = ttl::aligned_offset(address + 4, align2);
                    ttl::write_bytes(buff, address, In2);

                    return uint2(startAddress, size);
                }

                return uint2(0, 0);
            }

            template<typename T>
            uint2 AddAssertData(T In)
            {
                const uint size1 = ttl::bytes_required(In);
                const uint align1 = ttl::alignment_required(In);
                const uint sizeAndAlign1 = (size1 << 16) | align1;

                const uint alignedSize1 = ttl::aligned_offset(size1 + 4, align1);
                const uint size = ttl::aligned_offset(alignedSize1, 8);
                uint offset = 0;
                GetAllocationBuffer().InterlockedAdd(AssertDataSizeIndex, size, offset);

                const uint startAddress = Asserts.BeginData() + offset;
                uint address = startAddress;
                if (offset + size < Asserts.SizeInBytesOfData())
                {
                    RWByteAddressBuffer buff = GetTestDataBuffer();
                    ttl::write_bytes(buff, address, sizeAndAlign1);
                    address = ttl::aligned_offset(address + 4, align1);
                    ttl::write_bytes(buff, address, In);

                    return uint2(startAddress, size);
                }

                return uint2(0, 0);
            }

            template<typename T, typename = void>
            struct ByteReaderTraitsEval
            {
                static const uint16_t ReaderId = stf::ByteReaderTraits<T>::ReaderId;
                static const uint16_t TypeId = 0;
            };

            template<typename T>
            struct ByteReaderTraitsEval<T, ttl::enable_if_t<stf::ByteReaderTraits<T>::TypeId != 0> > : stf::ByteReaderTraits<T>{};
            
            template<typename T>
            void AddError(T In1, T In2, int InId)
            {
                const uint metaIndex = AddAssert();
                if (metaIndex < Asserts.Num())
                {
                    uint2 addressAndSize = uint2(0, 0);
                    if (Asserts.SizeInBytesOfData() > 0)
                    {
                        addressAndSize = AddAssertData(In1, In2);
                    }

                    using Traits = ByteReaderTraitsEval<T>;
                    const uint32_t readerId = Traits::ReaderId;
                    const uint32_t typeId = Traits::TypeId;
                    const uint packed = typeId | (readerId << 16);
                    AddAssertMetaInfo(metaIndex, InId, packed, addressAndSize);
                }
            }

            template<typename T>
            void AddError(T In, int InId)
            {
                const uint metaIndex = AddAssert();
                if (metaIndex < Asserts.Num())
                {
                    uint2 addressAndSize = uint2(0, 0);
                    if (Asserts.SizeInBytesOfData() > 0)
                    {
                        addressAndSize = AddAssertData(In);
                    }

                    using Traits = ByteReaderTraitsEval<T>;
                    const uint32_t readerId = Traits::ReaderId;
                    const uint32_t typeId = Traits::TypeId;
                    const uint packed = typeId | (readerId << 16);
                    AddAssertMetaInfo(metaIndex, InId, packed, addressAndSize);
                }
            }
        }
        

        template<typename T>
        void AreEqual(const T InA, const T InB, int InId = -1)
        {
            if (all(InA == InB))
            {
                detail::Success();
            }
            else
            {
                detail::AddError(InA, InB, InId);
            }
        }
        
        template<typename T>
        void NotEqual(const T InA, const T InB, int InId = -1)
        {
            if (any(InA != InB))
            {
                detail::Success();
            }
            else
            {
                detail::AddError(InA, InB, InId);
            }
        }
        
        template<typename T>
        void IsTrue(T In, int InId = -1)
        {
            if (ttl::cast<bool>(In))
            {
                detail::Success();
            }
            else
            {
                detail::AddError(In, InId);
            }
        }
        
        template<typename T>
        void IsFalse(T In, int InId = -1)
        {
            if (!ttl::cast<bool>(In))
            {
                detail::Success();
            }
            else
            {
                detail::AddError(In, InId);
            }
        }
        
        void Fail(int InId = -1)
        {
            detail::AddError(false, InId);
        }    
    }
}

#define STF_ASSERT_IF_0(InName, InId) stf::AssertionsV1::InName(InId)
#define STF_ASSERT_IF_1(InName, InId, InArg) stf::AssertionsV1::InName(InArg, InId)
#define STF_ASSERT_IF_2(InName, InId, InArg1, InArg2) stf::AssertionsV1::InName(InArg1, InArg2, InId)
#define ASSERT(InName, ...) TTL_JOIN_MACRO(STF_ASSERT_IF_, TTL_NUM_ARGS(__VA_ARGS__), InName, __LINE__, ##__VA_ARGS__)

#endif
