#pragma once

#include "D3D12/Fence.h"
#include "Utility/Object.h"
#include "Utility/Pointer.h"
#include "Utility/Time.h"

#include <d3d12.h>

namespace stf
{
    class CommandList;

    class CommandQueue 
        : public Object
    {
    public:

        struct CreationParams
        {
            ComPtr<ID3D12CommandQueue> Queue;
            SharedPtr<Fence> Fence{};
        };

        CommandQueue(ObjectToken, CreationParams InParams);
        ~CommandQueue();

        bool HasFencePointBeenReached(const Fence::FencePoint& InFencePoint) const;
        [[nodiscard]] Fence::FencePoint Signal();
        [[nodiscard]] Fence::FencePoint NextSignal();
        Fence::Expected<Fence::ECPUWaitResult> WaitOnFenceCPU(const Fence::FencePoint& InFencePoint);
        Fence::Expected<Fence::ECPUWaitResult> WaitOnFenceCPU(const Fence::FencePoint& InFencePoint, const Milliseconds<u32> InTimeout);
        void WaitOnFenceGPU(const Fence::FencePoint& InFencePoint);
        void SyncWithQueue(CommandQueue& InQueue);
        void FlushQueue();

        void ExecuteCommandList(CommandList& InList);

        ID3D12CommandQueue* GetRaw() const;
        operator ID3D12CommandQueue* () const;
        const Fence& GetFence() const;

        D3D12_COMMAND_LIST_TYPE GetType() const;

    private:

        ComPtr<ID3D12CommandQueue> m_Queue = nullptr;
        SharedPtr<Fence> m_Fence = nullptr;
    };
}