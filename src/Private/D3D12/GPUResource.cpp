#include "D3D12/GPUResource.h"

#include "Utility/Exception.h"

namespace stf
{
    GPUResource::GPUResource(CreationParams InParams) noexcept
        : m_Resource(std::move(InParams.Resource))
        , m_ClearValue(InParams.ClearValue)
        , m_CurrentBarrier(InParams.InitialBarrier)
    {
    }

    ID3D12Resource2* GPUResource::GetRaw() const noexcept
    {
        return m_Resource.Get();
    }

    GPUResource::operator ID3D12Resource2* () const noexcept
    {
        return GetRaw();
    }

    void GPUResource::SetBarrier(const GPUEnhancedBarrier InBarrier) noexcept
    {
        m_CurrentBarrier = InBarrier;
    }

    GPUEnhancedBarrier GPUResource::GetBarrier() const noexcept
    {
        return m_CurrentBarrier;
    }

    D3D12_RESOURCE_DESC1 GPUResource::GetDesc() const noexcept
    {
        return m_Resource->GetDesc1();
    }

    std::optional<D3D12_CLEAR_VALUE> GPUResource::GetClearValue() const noexcept
    {
        return m_ClearValue;
    }

    u64 GPUResource::GetGPUAddress() const noexcept
    {
        return m_Resource->GetGPUVirtualAddress();
    }

    MappedResource GPUResource::Map() const
    {
        return MappedResource(GPUResourceToken{}, m_Resource);
    }

    MappedResource::MappedResource(GPUResourceToken, ComPtr<ID3D12Resource2> InResource)
        : m_Resource(std::move(InResource))
        , m_MappedData()
    {
        if (m_Resource)
        {
            D3D12_RANGE range{ 0, m_Resource->GetDesc().Width };
            void* mappedData = nullptr;
            ThrowIfFailed(m_Resource->Map(0, &range, &mappedData));
            m_MappedData = std::span<const std::byte>(static_cast<const std::byte*>(mappedData), range.End);
        }
    }

    MappedResource::~MappedResource()
    {
        if (m_Resource)
        {
            m_Resource->Unmap(0, nullptr);
        }
    }
}
