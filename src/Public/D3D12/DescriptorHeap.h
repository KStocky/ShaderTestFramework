#pragma once

#include "Platform.h"
#include "D3D12/Descriptor.h"
#include "Utility/Expected.h"
#include "Utility/Object.h"
#include "Utility/Pointer.h"

#include <d3d12.h>

namespace stf
{
    class DescriptorHeap : Object
    {
    public:

        struct Desc
        {
            ComPtr<ID3D12DescriptorHeap> Heap;
            u32 DescriptorSize = 0;
        };

        enum class EErrorType : u8
        {
            AllocationError
        };

        template<typename T>
        using Expected = Expected<T, EErrorType>;

        DescriptorHeap() = default;
        DescriptorHeap(Desc InParams) noexcept;

        Expected<DescriptorRange> CreateDescriptorRange(const u32 InBeginIndex, const u32 InNum) const;
        Expected<DescriptorHandle> CreateDescriptorHandle(const u32 InIndex) const;

        u32 GetNumDescriptors() const noexcept;
        D3D12_DESCRIPTOR_HEAP_TYPE GetType() const noexcept;
        D3D12_DESCRIPTOR_HEAP_FLAGS GetAccess() const;

        ID3D12DescriptorHeap* GetRaw() const
        {
            return m_Heap.Get();
        }

        operator ID3D12DescriptorHeap*() const
        {
            return GetRaw();
        }

    private:

        ComPtr<ID3D12DescriptorHeap> m_Heap = nullptr;
        u32 m_DescriptorSize = 0;
    };
}