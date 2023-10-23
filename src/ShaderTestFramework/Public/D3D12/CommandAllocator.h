#pragma once

#include "Platform.h"
#include "Utility/MoveOnly.h"
#include "Utility/Pointer.h"

#include <d3d12.h>

class CommandAllocator : MoveOnly
{
public:

	struct CreationParams
	{
		ComPtr<ID3D12CommandAllocator> Allocator = nullptr;
		D3D12_COMMAND_LIST_TYPE Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	};

	CommandAllocator(CreationParams InParams);

	ID3D12CommandAllocator* GetRaw() const;
	operator ID3D12CommandAllocator* () const;

	D3D12_COMMAND_LIST_TYPE GetType() const;

	ExpectedHRes<void> Reset();

private:

	ComPtr<ID3D12CommandAllocator> m_Allocator;
	D3D12_COMMAND_LIST_TYPE m_Type;
};