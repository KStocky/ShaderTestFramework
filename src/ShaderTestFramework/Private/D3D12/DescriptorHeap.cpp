#include "D3D12/DescriptorHeap.h"

DescriptorHeap::DescriptorHeap(Desc InParams) noexcept
	: m_Heap(std::move(InParams.Heap))
	, m_DescriptorSize(InParams.DescriptorSize)
{
}

DescriptorHeap::Expected<DescriptorRange> DescriptorHeap::CreateDescriptorRange(const u32 InBeginIndex, const u32 InNum)
{
	if ((InBeginIndex + InNum) > GetNumDescriptors())
	{
		return std::unexpected(EErrorType::AllocationError);
	}

	const u64 offset = InBeginIndex * m_DescriptorSize;

	const u64 gpuPtr = GetAccess() == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE ? m_Heap->GetGPUDescriptorHandleForHeapStart().ptr : 0ull;
	const D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{ m_Heap->GetCPUDescriptorHandleForHeapStart().ptr + offset };
	const D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{ gpuPtr + offset };

	return DescriptorRange(DescriptorHandle{ cpuHandle, gpuHandle }, InNum, m_DescriptorSize);
}

DescriptorHeap::Expected<DescriptorHandle> DescriptorHeap::CreateDescriptorHandle(const u32 InIndex)
{
	return CreateDescriptorRange(InIndex, 1).and_then([](const DescriptorRange& InRange) { return Expected<DescriptorHandle>{ *InRange[0] }; });
}

u32 DescriptorHeap::GetNumDescriptors() const noexcept
{
	return m_Heap->GetDesc().NumDescriptors;
}

D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeap::GetType() const noexcept
{
	return m_Heap->GetDesc().Type;
}

D3D12_DESCRIPTOR_HEAP_FLAGS DescriptorHeap::GetAccess() const
{
	return m_Heap->GetDesc().Flags;
}
