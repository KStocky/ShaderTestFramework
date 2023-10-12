#pragma once

#include "Container/RingBuffer.h"
#include "D3D12/Descriptor.h"

#include "Expected.h"

class DescriptorFreeListAllocator
{
public:

	enum class EErrorType
	{
		EmptyError,
		InvalidDescriptor
	};

	template<typename T>
	using Expected = Expected<T, EErrorType>;

	DescriptorFreeListAllocator() = default;
	DescriptorFreeListAllocator(const DescriptorRange& InRange);
	[[nodiscard]] Expected<DescriptorHandle> Allocate();
	Expected<void> Release(const DescriptorHandle& InDescriptor);

	u32 GetSize() const;
	u32 GetCapacity() const;

private:

	DescriptorRange m_Range;
	RingBuffer<u32> m_FreeList;
	std::vector<u8> m_FreeSet;
};