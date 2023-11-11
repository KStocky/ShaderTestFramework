#include "D3D12/CommandList.h"

#include "D3D12/CommandAllocator.h"
#include "Utility/Exception.h"

CommandList::CommandList(CreationParams InParams)
	: m_List(std::move(InParams.List))
{
}

void CommandList::Close()
{
	ThrowIfFailed(m_List->Close());
}

void CommandList::Reset(CommandAllocator& InAllocator)
{
	ThrowIfUnexpected(InAllocator.Reset());
	ThrowIfFailed(m_List->Reset(InAllocator, nullptr));
}

D3D12_COMMAND_LIST_TYPE CommandList::GetType() const
{
	return m_List->GetType();
}

ID3D12CommandList* CommandList::GetRaw() const
{
	return m_List.Get();
}

CommandList::operator ID3D12CommandList* () const
{
	return GetRaw();
}
