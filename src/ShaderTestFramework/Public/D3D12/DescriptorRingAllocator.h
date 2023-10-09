#pragma once

#include "D3D12/Descriptor.h"
#include "Platform.h"

#include <expected>
#include <utility>

class DescriptorRingAllocator
{
public:

	enum class EErrorType : u8
	{
		Success,
		AllocationError,
		ReleaseError
	};

	template<typename T>
	using Expected = std::expected<T, EErrorType>;

	DescriptorRingAllocator() = default;
	DescriptorRingAllocator(const DescriptorRange& InRange);
	[[nodiscard]] Expected<DescriptorRange> Allocate(const u32 InNum);
	Expected<void> Release(const u32 InNum);

	u32 GetSize() const;
	u32 GetCapacity() const;

	bool CanAllocate(const u32 InNum) const;

private:
	bool CanAllocateInternal(const u32 InActualNum) const;
	std::pair<u32, u32> CalculateAllocationIndexAndSize(const u32 InRequestedNum) const;

	DescriptorRange m_Range;
	u32 m_Size = 0;
	u32 m_AllocateIndex = 0;
	u32 m_ReleaseIndex = 0;
};