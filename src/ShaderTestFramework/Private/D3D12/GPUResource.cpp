#include "D3D12/GPUResource.h"

GPUResource::GPUResource(CreationParams InParams) noexcept
	: m_Resource(std::move(InParams.Resource))
	, m_ClearValue(InParams.ClearValue)
	, m_CurrentBarrier(InParams.InitialBarrier)
{
}

ID3D12Resource2* GPUResource::GetRaw() const noexcept
{
	return m_Resource.Get();
}

GPUResource::operator ID3D12Resource2* () const noexcept
{
	return GetRaw();
}

void GPUResource::SetBarrier(const GPUEnhancedBarrier InBarrier) noexcept
{
	m_CurrentBarrier = InBarrier;
}

GPUEnhancedBarrier GPUResource::GetBarrier() const noexcept
{
	return m_CurrentBarrier;
}

D3D12_RESOURCE_DESC1 GPUResource::GetDesc() const noexcept
{
	return m_Resource->GetDesc1();
}

std::optional<D3D12_CLEAR_VALUE> GPUResource::GetClearValue() const noexcept
{
	return m_ClearValue;
}

u64 GPUResource::GetGPUAddress() const noexcept
{
	return m_Resource->GetGPUVirtualAddress();
}
