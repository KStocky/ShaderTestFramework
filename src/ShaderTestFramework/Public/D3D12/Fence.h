#pragma once
#include "Utility/MoveOnly.h"
#include "Platform.h"
#include "Utility/Pointer.h"

#include <d3d12.h>

class Fence : MoveOnly
{
public:

	struct FencePoint
	{
		friend class Fence;
		FencePoint() = default;
	private:

		FencePoint(ID3D12Fence1* InFence, const u64 InValue)
			: Fence(InFence)
			, SignalledValue(InValue)
		{
		}

		ID3D12Fence1* Fence = nullptr;
		u64 SignalledValue = 0;
	};

	struct CreationParams
	{
		ComPtr<ID3D12Fence1> Fence = nullptr;
		u64 InitialValue = 0;
	};

	Fence() = default;
	Fence(CreationParams InParams);
	Fence(Fence&& In) noexcept;

	Fence& operator=(Fence&& In) noexcept;

	[[nodiscard]] FencePoint Signal(ID3D12CommandQueue* InQueue);
	Expected<void, bool> WaitCPU(const FencePoint& InFencePoint) const;
	void WaitOnQueue(ID3D12CommandQueue* InQueue) const;
	Expected<bool, bool> HasCompleted(const FencePoint& InFencePoint) const;

	ID3D12Fence* GetRaw() const;
	operator ID3D12Fence* () const;

private:

	bool Validate(const FencePoint& InFencePoint) const;

	ComPtr<ID3D12Fence1> m_Fence = nullptr;
	u64 m_NextValue = 0;
};