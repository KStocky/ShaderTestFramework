#pragma once

#include "Utility/MoveOnly.h"
#include "Utility/Pointer.h"

#include <d3d12.h>

namespace stf
{
    class PipelineState : MoveOnly
    {
    public:

        struct CreationParams
        {
            ComPtr<ID3D12PipelineState> Raw = nullptr;
        };

        PipelineState() = default;
        PipelineState(CreationParams InParams) noexcept;

        template<typename ThisType>
        ID3D12PipelineState* GetRaw(this ThisType&& InThis)
        {
            return std::forward<ThisType>(InThis).m_Raw.Get();
        }

        template<typename ThisType>
        operator ID3D12PipelineState* (this ThisType&& InThis)
        {
            return std::forward<ThisType>(InThis).GetRaw();
        }

        D3D12_CACHED_PIPELINE_STATE GetCachedState() const;

    private:
        ComPtr<ID3D12PipelineState> m_Raw = nullptr;
    };
}