#pragma once

#include "Utility/MoveOnly.h"
#include "Platform.h"
#include "Utility/Pointer.h"

#include <d3d12.h>

class CommandAllocator;

class CommandList : MoveOnly
{
public:

	struct CreationParams
	{
		ComPtr<ID3D12GraphicsCommandList9> List = nullptr;
	};

	CommandList() = default;
	CommandList(CreationParams InParams);

	ExpectedHRes<void> Close();
	ExpectedHRes<void> Reset(CommandAllocator& InAllocator);

	D3D12_COMMAND_LIST_TYPE GetType() const;

	ID3D12CommandList* GetRaw() const;
	operator ID3D12CommandList* () const;

private:

	ComPtr<ID3D12GraphicsCommandList9> m_List = nullptr;
};