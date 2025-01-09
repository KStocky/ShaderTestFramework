#include "D3D12/Fence.h"
#include "Utility/Exception.h"

namespace stf
{
    Fence::Fence(ObjectToken InToken, CreationParams InParams)
        : Object(InToken)
        , m_Fence(std::move(InParams.Fence))
        , m_NextValue(InParams.InitialValue + 1)
    {
    }

    Fence::FencePoint Fence::Signal(ID3D12CommandQueue& InQueue)
    {
        InQueue.Signal(m_Fence.Get(), m_NextValue);
        return { m_Fence.Get(), m_NextValue++ };
    }

    Fence::FencePoint Fence::NextSignal()
    {
        return FencePoint{ m_Fence.Get(), m_NextValue };
    }

    Fence::Expected<Fence::ECPUWaitResult> Fence::WaitCPU(const FencePoint& InFencePoint) const
    {
        return WaitForFencePoint(InFencePoint, INFINITE);
    }

    Fence::Expected<Fence::ECPUWaitResult> Fence::WaitCPU(const FencePoint& InFencePoint, const Milliseconds<u32> InTimeOut) const
    {
        return WaitForFencePoint(InFencePoint, InTimeOut.count());
    }

    void Fence::WaitOnQueue(ID3D12CommandQueue& InQueue, const FencePoint& InFencePoint) const
    {
        ThrowIfFailed(InQueue.Wait(InFencePoint.Fence, InFencePoint.SignalledValue));
    }

    Fence::Expected<bool> Fence::HasCompleted(const FencePoint& InFencePoint) const
    {
        return Validate(InFencePoint)
            .transform(
                [&, this]()
                {
                    return m_Fence->GetCompletedValue() >= InFencePoint.SignalledValue;
                }
            );
    }

    ID3D12Fence* Fence::GetRaw() const
    {
        return m_Fence.Get();
    }

    Fence::operator ID3D12Fence* () const
    {
        return GetRaw();
    }

    Fence::Expected<void> Fence::Validate(const FencePoint& InFencePoint) const
    {
        if (m_Fence.Get() != InFencePoint.Fence)
        {
            return Unexpected{ EErrorType::FencePointIsFromAnotherFence };
        }

        return {};
    }

    Fence::Expected<Fence::ECPUWaitResult> Fence::WaitForFencePoint(const FencePoint& InFencePoint, const DWORD InTimeout) const
    {
        
        return Validate(InFencePoint)
            .and_then(
                [&, this]() -> Expected<ECPUWaitResult>
                {
                    if (m_Fence->GetCompletedValue() > InFencePoint.SignalledValue)
                    {
                        return ECPUWaitResult::FenceAlreadyReached;
                    }

                    HANDLE event = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
                    if (!event)
                    {
                        return Unexpected{ EErrorType::FencePointIsFromAnotherFence };
                    }

                    m_Fence->SetEventOnCompletion(InFencePoint.SignalledValue, event);
                    const auto waitResult = WaitForSingleObject(event, InTimeout);

                    CloseHandle(event);

                    switch (waitResult)
                    {
                        case WAIT_OBJECT_0:
                        {
                            return ECPUWaitResult::WaitFenceReached;
                        }

                        case WAIT_TIMEOUT:
                        {
                            return ECPUWaitResult::WaitTimedOut;
                        }

                        default:
                        {
                            return Unexpected{ EErrorType::WaitFailed };
                        }
                    }
                });
    }
}
