#include "D3D12/CommandList.h"

#include "D3D12/CommandAllocator.h"
#include "D3D12/DescriptorHeap.h"
#include "D3D12/Shader/PipelineState.h"
#include "D3D12/Shader/RootSignature.h"
#include "Utility/Exception.h"

CommandList::CommandList(CreationParams InParams)
	: m_List(std::move(InParams.List))
{
}

void CommandList::Dispatch(const u32 InX, const u32 InY, const u32 InZ)
{
	m_List->Dispatch(InX, InY, InZ);
}

void CommandList::SetDescriptorHeaps(const DescriptorHeap& InHeap)
{
	const auto rawHeap = InHeap.GetRaw();
	m_List->SetDescriptorHeaps(1, &rawHeap);
}

void CommandList::SetGraphicsRootSignature(const RootSignature& InRootSig)
{
	m_List->SetGraphicsRootSignature(InRootSig);
}

void CommandList::SetPipelineState(const PipelineState& InState)
{
	m_List->SetPipelineState(InState);
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
