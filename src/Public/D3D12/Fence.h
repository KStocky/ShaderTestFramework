#pragma once
#include "Platform.h"

#include "Utility/Expected.h"
#include "Utility/Object.h"
#include "Utility/Pointer.h"
#include "Utility/Time.h"
#include <d3d12.h>

namespace stf
{
    class Fence 
        : public Object
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

        enum class EErrorType
        {
            FencePointIsFromAnotherFence,
            ErrorCreatingEvent,
            WaitFailed
        };

        enum class ECPUWaitResult
        {
            FenceAlreadyReached,
            WaitTimedOut,
            WaitFenceReached
        };

        template<typename T>
        using Expected = Expected<T, EErrorType>;

        Fence(ObjectToken, CreationParams InParams);

        [[nodiscard]] FencePoint Signal(ID3D12CommandQueue& InQueue);
        [[nodiscard]] FencePoint NextSignal();
        Expected<ECPUWaitResult> WaitCPU(const FencePoint& InFencePoint) const;
        Expected<ECPUWaitResult> WaitCPU(const FencePoint& InFencePoint, const Milliseconds<u32> InTimeout) const;
        void WaitOnQueue(ID3D12CommandQueue& InQueue, const FencePoint& InFencePoint) const;
        Expected<bool> HasCompleted(const FencePoint& InFencePoint) const;

        ID3D12Fence* GetRaw() const;
        operator ID3D12Fence* () const;

    private:

        Expected<void> Validate(const FencePoint& InFencePoint) const;

        Expected<ECPUWaitResult> WaitForFencePoint(const FencePoint& InFencePoint, DWORD InTimeout) const;

        ComPtr<ID3D12Fence1> m_Fence = nullptr;
        u64 m_NextValue = 0;
    };
}