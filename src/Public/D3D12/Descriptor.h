#pragma once

#include "Utility/Expected.h"
#include "Platform.h"

#include <d3d12.h>

namespace stf
{
    class DescriptorHandle
    {
    public:

        DescriptorHandle() = default;

        DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE InCPUHandle, D3D12_GPU_DESCRIPTOR_HANDLE InGPUHandle, u32 InHeapIndex)
            : m_CPUAddress(InCPUHandle)
            , m_GPUAddress(InGPUHandle)
            , m_HeapIndex(InHeapIndex)
        {
        }

        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const
        {
            return m_CPUAddress;
        }

        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const
        {
            return m_GPUAddress;
        }

        u32 GetHeapIndex() const
        {
            return m_HeapIndex;
        }

    private:

        D3D12_CPU_DESCRIPTOR_HANDLE m_CPUAddress{ 0 };
        D3D12_GPU_DESCRIPTOR_HANDLE m_GPUAddress{ 0 };
        u32 m_HeapIndex{ 0 };
    };

    class DescriptorRange
    {
    public:

        enum class EErrorType
        {
            Success,
            InvalidRange,
            InvalidSubrange,
            InvalidIndex,
            InvalidHandle
        };

        template<typename T>
        using Expected = Expected<T, EErrorType>;

        DescriptorRange() = default;
        DescriptorRange(DescriptorHandle InStart, const u32 InSize, const u32 InIncrement)
            : m_Start(InStart)
            , m_Size(InSize)
            , m_Increment(InIncrement)
        {}

        Expected<DescriptorHandle> First() const
        {
            if (!IsValidRange())
            {
                return Unexpected(EErrorType::InvalidRange);
            }
            return m_Start;
        }

        Expected<DescriptorHandle> operator[](const u32 InIndex) const
        {
            if (!IsValidRange())
            {
                return Unexpected(EErrorType::InvalidRange);
            }
            if (InIndex >= m_Size)
            {
                return Unexpected(EErrorType::InvalidIndex);
            }

            D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{ m_Start.GetCPUHandle().ptr + m_Increment * InIndex };
            D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{ m_Start.GetGPUHandle().ptr + m_Increment * InIndex };
            return DescriptorHandle(cpuHandle, gpuHandle, m_Start.GetHeapIndex() + InIndex);
        }

        u32 GetSize() const
        {
            return m_Size;
        }

        Expected<DescriptorRange> Subrange(const u32 InStartIndex, const u32 InSize) const
        {
            if (!IsValidRange())
            {
                return Unexpected(EErrorType::InvalidRange);
            }
            if (InStartIndex + InSize > m_Size)
            {
                return Unexpected(EErrorType::InvalidSubrange);
            }

            return DescriptorRange{ *(*this)[InStartIndex], InSize, m_Increment };
        }

        Expected<u32> GetIndex(const DescriptorHandle& InHandle) const
        {
            if (!IsValidRange())
            {
                return Unexpected(EErrorType::InvalidRange);
            }

            const auto startRangeAddress = m_Start.GetCPUHandle();
            const auto handleAddress = InHandle.GetCPUHandle();

            if (startRangeAddress.ptr > handleAddress.ptr)
            {
                return Unexpected(EErrorType::InvalidHandle);
            }
            const auto diff = handleAddress.ptr - startRangeAddress.ptr;
            const u32 ret = static_cast<u32>(diff / m_Increment);
            const u32 remainder = static_cast<u32>(diff % m_Increment);

            if (ret >= m_Size || remainder != 0)
            {
                return Unexpected(EErrorType::InvalidHandle);
            }

            return ret;
        }

        bool IsValidRange() const
        {
            return m_Size != 0 && m_Increment != 0;
        }

    private:
        DescriptorHandle m_Start;
        u32 m_Size = 0;
        u32 m_Increment = 0;
    };
}