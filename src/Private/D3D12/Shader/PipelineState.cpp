#include "D3D12/Shader/PipelineState.h"

#include "Utility/Exception.h"

namespace stf
{
    PipelineState::PipelineState(ObjectToken InToken, CreationParams InParams) noexcept
        : Object(InToken)
        , m_Raw(std::move(InParams.Raw))
    {
    }

    D3D12_CACHED_PIPELINE_STATE PipelineState::GetCachedState() const
    {
        ComPtr<ID3DBlob> blob;
        ThrowIfFailed(m_Raw->GetCachedBlob(blob.GetAddressOf()));
        return D3D12_CACHED_PIPELINE_STATE
        {
            .pCachedBlob = blob->GetBufferPointer(),
            .CachedBlobSizeInBytes = blob->GetBufferSize()
        };
    }
}
