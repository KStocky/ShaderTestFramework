#include "D3D12/CommandQueue.h"

#include "D3D12/CommandList.h"

#include "Utility/Exception.h"

namespace stf
{
    CommandQueue::CommandQueue(ObjectToken InToken, CreationParams InParams)
        : Object(InToken)
        , m_Queue(std::move(InParams.Queue))
        , m_Fence(std::move(InParams.Fence))
    {
    }

    CommandQueue::~CommandQueue()
    {
        FlushQueue();
    }

    bool CommandQueue::HasFencePointBeenReached(const Fence::FencePoint& InFencePoint) const
    {
        return ThrowIfUnexpected(m_Fence->HasCompleted(InFencePoint));
    }

    Fence::FencePoint CommandQueue::Signal()
    {
        return m_Fence->Signal(*m_Queue.Get());
    }

    void CommandQueue::WaitOnFence(const Fence::FencePoint& InFencePoint)
    {
        m_Fence->WaitCPU(InFencePoint);
    }

    void CommandQueue::SyncWithQueue(CommandQueue& InQueue)
    {
        InQueue.GetFence().WaitOnQueue(*m_Queue.Get(), Signal());
    }

    void CommandQueue::FlushQueue()
    {
        WaitOnFence(Signal());
    }

    void CommandQueue::ExecuteCommandList(CommandList& InList)
    {
        InList.Close();
        ID3D12CommandList* rawList = InList.GetRaw();
        m_Queue->ExecuteCommandLists(1, &rawList);
    }

    ID3D12CommandQueue* CommandQueue::GetRaw() const
    {
        return m_Queue.Get();
    }

    CommandQueue::operator ID3D12CommandQueue* () const
    {
        return GetRaw();
    }

    const Fence& CommandQueue::GetFence() const
    {
        return *m_Fence;
    }

    D3D12_COMMAND_LIST_TYPE CommandQueue::GetType() const
    {
        return m_Queue->GetDesc().Type;
    }
}
