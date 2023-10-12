#pragma once

#include "D3D12/Descriptor.h"
#include "Expected.h"
#include "MoveOnly.h"
#include "Platform.h"
#include "Pointer.h"

#include <d3d12.h>

class DescriptorHeap : MoveOnly
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

	Expected<DescriptorRange> CreateDescriptorRange(const u32 InBeginIndex, const u32 InNum);
	Expected<DescriptorHandle> CreateDescriptorHandle(const u32 InIndex);

	u32 GetNumDescriptors() const noexcept;
	D3D12_DESCRIPTOR_HEAP_TYPE GetType() const noexcept;
	D3D12_DESCRIPTOR_HEAP_FLAGS GetAccess() const;

private:

	ComPtr<ID3D12DescriptorHeap> m_Heap = nullptr;
	u32 m_DescriptorSize = 0;
};