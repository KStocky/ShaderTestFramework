#include "D3D12/CommandAllocator.h"

#include "Utility/Exception.h"

namespace stf
{
    CommandAllocator::CommandAllocator(CreationParams InParams)
        : m_Allocator(std::move(InParams.Allocator))
        , m_Type(InParams.Type)
    {
    }

    ID3D12CommandAllocator* CommandAllocator::GetRaw() const
    {
        return m_Allocator.Get();
    }

    CommandAllocator::operator ID3D12CommandAllocator* () const
    {
        return GetRaw();
    }

    D3D12_COMMAND_LIST_TYPE CommandAllocator::GetType() const
    {
        return m_Type;
    }

    void CommandAllocator::Reset()
    {
        ThrowIfFailed(m_Allocator->Reset());
    }
}
