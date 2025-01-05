#pragma once

#include "Utility/Object.h"
#include "Utility/Pointer.h"

#include <d3d12.h>

namespace stf
{
    class PipelineState 
        : public Object
    {
    public:

        struct CreationParams
        {
            ComPtr<ID3D12PipelineState> Raw = nullptr;
        };

        PipelineState(ObjectToken, CreationParams InParams) noexcept;

        ID3D12PipelineState* GetRaw() const
        {
            return m_Raw.Get();
        }

        operator ID3D12PipelineState* () const
        {
            return GetRaw();
        }

        D3D12_CACHED_PIPELINE_STATE GetCachedState() const;

    private:
        ComPtr<ID3D12PipelineState> m_Raw = nullptr;
    };
}