#pragma once

#include "D3D12/Fence.h"
#include "Utility/MoveOnly.h"
#include "Utility/Pointer.h"

#include <d3d12.h>

class CommandList;

class CommandQueue : MoveOnly
{
public:

	struct CreationParams
	{
		ComPtr<ID3D12CommandQueue> Queue;
		Fence Fence{};
	};

	CommandQueue() = default;
	CommandQueue(CreationParams InParams);

	bool HasFencePointBeenReached(const Fence::FencePoint& InFencePoint) const;
	Fence::FencePoint Signal();
	void WaitOnFence(const Fence::FencePoint& InFencePoint);
	void SyncWithQueue(CommandQueue& InQueue);
	void FlushQueue();

	void ExecuteCommandList(CommandList& InList);

	ID3D12CommandQueue* GetRaw() const;
	operator ID3D12CommandQueue* () const;
	const Fence& GetFence() const;

	D3D12_COMMAND_LIST_TYPE GetType() const;

private:

	ComPtr<ID3D12CommandQueue> m_Queue = nullptr;
	Fence m_Fence;
};