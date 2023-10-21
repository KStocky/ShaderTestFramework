#pragma once

#include "Utility/MoveOnly.h"
#include "Platform.h"
#include "Utility/Pointer.h"

#include <optional>

#include <d3d12.h>

struct GPUEnhancedBarrier
{
	D3D12_BARRIER_SYNC Sync{};
	D3D12_BARRIER_ACCESS Access{};
	D3D12_BARRIER_LAYOUT Layout{};
};

class GPUResource : MoveOnly
{
public:

	struct CreationParams
	{
		ComPtr<ID3D12Resource2> Resource;
		std::optional<D3D12_CLEAR_VALUE> ClearValue{};
		GPUEnhancedBarrier InitialBarrier{};
	};

	GPUResource() = default;
	GPUResource(CreationParams InParams) noexcept;

	ID3D12Resource2* GetRaw() const noexcept;
	operator ID3D12Resource2* () const noexcept;

	void SetBarrier(const GPUEnhancedBarrier InBarrier) noexcept;
	GPUEnhancedBarrier GetBarrier() const noexcept;
	D3D12_RESOURCE_DESC1 GetDesc() const noexcept;
	std::optional<D3D12_CLEAR_VALUE> GetClearValue() const noexcept;

	u64 GetGPUAddress() const noexcept;

private:

	ComPtr<ID3D12Resource2> m_Resource;
	std::optional<D3D12_CLEAR_VALUE> m_ClearValue{};
	GPUEnhancedBarrier m_CurrentBarrier;
};