#include "D3D12/Fence.h"

namespace stf
{
    Fence::Fence(CreationParams InParams)
        : m_Fence(std::move(InParams.Fence))
        , m_NextValue(InParams.InitialValue + 1)
    {
    }

    Fence::FencePoint Fence::Signal(ID3D12CommandQueue* InQueue)
    {
        InQueue->Signal(m_Fence.Get(), m_NextValue);
        return { m_Fence.Get(), m_NextValue++ };
    }

    Expected<void, bool> Fence::WaitCPU(const FencePoint& InFencePoint) const
    {
        if (!Validate(InFencePoint))
        {
            return Unexpected{ false };
        }
        const u64 currentValue = m_Fence->GetCompletedValue();

        if (currentValue < InFencePoint.SignalledValue)
        {
            HANDLE event = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

            m_Fence->SetEventOnCompletion(InFencePoint.SignalledValue, event);
            WaitForSingleObject(event, INFINITE);
        }

        return{};
    }

    void Fence::WaitOnQueue(ID3D12CommandQueue* InQueue) const
    {
        InQueue->Wait(m_Fence.Get(), m_NextValue - 1ull);
    }

    Expected<bool, bool> Fence::HasCompleted(const FencePoint& InFencePoint) const
    {
        if (!Validate(InFencePoint))
        {
            return Unexpected{ false };
        }
        return m_Fence->GetCompletedValue() >= InFencePoint.SignalledValue;
    }

    ID3D12Fence* Fence::GetRaw() const
    {
        return m_Fence.Get();
    }

    Fence::operator ID3D12Fence* () const
    {
        return GetRaw();
    }

    bool Fence::Validate(const FencePoint& InFencePoint) const
    {
        return m_Fence.Get() == InFencePoint.Fence;
    }
}
