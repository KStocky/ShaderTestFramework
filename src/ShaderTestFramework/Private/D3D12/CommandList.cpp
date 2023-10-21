#include "D3D12/CommandList.h"

#include "D3D12/CommandAllocator.h"

CommandList::CommandList(CreationParams InParams)
	: m_List(std::move(InParams.List))
{
}

ExpectedHRes<void> CommandList::Close()
{
	if (const auto hres = m_List->Close();
		FAILED(hres))
	{
		return Unexpected{ hres };
	}

	return {};
}

ExpectedHRes<void> CommandList::Reset(CommandAllocator& InAllocator)
{
	if (const auto hres = m_List->Reset(InAllocator, nullptr);
		FAILED(hres))
	{
		return Unexpected{ hres };
	}

	return {};
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
