#include "D3D12/Shader/RootSignature.h"

namespace stf
{
    RootSignature::RootSignature(CreationParams InParams)
        : m_RootSig(std::move(InParams.RootSig))
        , m_Deserializer(std::move(InParams.Deserializer))
        , m_Blob(std::move(InParams.Blob))
    {
    }

    const D3D12_VERSIONED_ROOT_SIGNATURE_DESC* RootSignature::GetDesc() const
    {
        return m_Deserializer->GetUnconvertedRootSignatureDesc();
    }

    ID3D12RootSignature* RootSignature::GetRaw() const
    {
        return m_RootSig.Get();
    }

    RootSignature::operator ID3D12RootSignature* () const
    {
        return GetRaw();
    }
}
