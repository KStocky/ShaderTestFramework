#include "D3D12/CommandList.h"

#include "D3D12/CommandAllocator.h"
#include "D3D12/DescriptorHeap.h"
#include "D3D12/GPUResource.h"
#include "D3D12/Shader/PipelineState.h"
#include "D3D12/Shader/RootSignature.h"
#include "Utility/Exception.h"

#include <d3dx12/d3dx12.h>

namespace stf
{
    CommandList::CommandList(CreationParams InParams)
        : m_List(std::move(InParams.List))
    {
    }

    void CommandList::CopyBufferResource(GPUResource& InDest, GPUResource& InSource)
    {
        const auto prevSourceBarrier = InSource.GetBarrier();
        const auto prevDestBarrier = InDest.GetBarrier();

        GPUEnhancedBarrier newSourceBarrier
        {
            .Sync = D3D12_BARRIER_SYNC_COPY,
            .Access = D3D12_BARRIER_ACCESS_COPY_SOURCE,
            .Layout = D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_COPY_SOURCE
        };

        GPUEnhancedBarrier newDestBarrier
        {
            .Sync = D3D12_BARRIER_SYNC_COPY,
            .Access = D3D12_BARRIER_ACCESS_COPY_DEST,
            .Layout = D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_COPY_DEST
        };

        InSource.SetBarrier(newSourceBarrier);
        InDest.SetBarrier(newDestBarrier);

        D3D12_BUFFER_BARRIER bufferBarriers[] =
        {
            CD3DX12_BUFFER_BARRIER(
                prevSourceBarrier.Sync,
                newSourceBarrier.Sync,
                prevSourceBarrier.Access,
                newSourceBarrier.Access,
                InSource
            ),
            CD3DX12_BUFFER_BARRIER(
                prevDestBarrier.Sync,
                newDestBarrier.Sync,
                prevDestBarrier.Access,
                newDestBarrier.Access,
                InDest
            )
        };

        D3D12_BARRIER_GROUP BufBarrierGroups[] =
        {
            CD3DX12_BARRIER_GROUP(2, bufferBarriers)
        };

        m_List->Barrier(1, BufBarrierGroups);
        m_List->CopyResource(InDest, InSource);
    }

    void CommandList::Dispatch(const u32 InX, const u32 InY, const u32 InZ)
    {
        m_List->Dispatch(InX, InY, InZ);
    }

    void CommandList::SetComputeRootSignature(const RootSignature& InRootSig)
    {
        m_List->SetComputeRootSignature(InRootSig);
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

    void CommandList::SetBufferUAV(GPUResource& InResource)
    {
        const auto prevBarrier = InResource.GetBarrier();
        GPUEnhancedBarrier newBarrier
        {
            .Sync = D3D12_BARRIER_SYNC_COMPUTE_SHADING,
            .Access = D3D12_BARRIER_ACCESS_UNORDERED_ACCESS,
            .Layout = D3D12_BARRIER_LAYOUT_DIRECT_QUEUE_UNORDERED_ACCESS
        };

        InResource.SetBarrier(newBarrier);
        D3D12_BUFFER_BARRIER bufferBarriers[] =
        {
            CD3DX12_BUFFER_BARRIER(
                prevBarrier.Sync,
                newBarrier.Sync,
                prevBarrier.Access,
                newBarrier.Access,
                InResource
            )
        };

        D3D12_BARRIER_GROUP BufBarrierGroups[] =
        {
            CD3DX12_BARRIER_GROUP(1, bufferBarriers)
        };

        m_List->Barrier(1, BufBarrierGroups);
    }

    void CommandList::Close()
    {
        ThrowIfFailed(m_List->Close());
    }

    void CommandList::Reset(CommandAllocator& InAllocator)
    {
        InAllocator.Reset();
        ThrowIfFailed(m_List->Reset(InAllocator, nullptr));
    }

    D3D12_COMMAND_LIST_TYPE CommandList::GetType() const
    {
        return m_List->GetType();
    }

    ID3D12GraphicsCommandList* CommandList::GetRaw() const
    {
        return m_List.Get();
    }

    CommandList::operator ID3D12GraphicsCommandList* () const
    {
        return GetRaw();
    }
}

