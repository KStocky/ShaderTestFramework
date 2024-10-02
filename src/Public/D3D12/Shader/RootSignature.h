#pragma once

#include "Platform.h"
#include "Utility/MoveOnly.h"
#include "Utility/Pointer.h"

#include <d3d12.h>

namespace stf
{
    class RootSignature : MoveOnly
    {
    public:

        struct CreationParams
        {
            ComPtr<ID3D12RootSignature> RootSig;
            ComPtr<ID3D12VersionedRootSignatureDeserializer> Deserializer;
            ComPtr<ID3DBlob> Blob;
        };

        RootSignature() = default;
        RootSignature(CreationParams InParams);

        const D3D12_VERSIONED_ROOT_SIGNATURE_DESC* GetDesc() const;
        ID3D12RootSignature* GetRaw() const;
        operator ID3D12RootSignature* () const;

    private:

        ComPtr<ID3D12RootSignature> m_RootSig;
        ComPtr<ID3D12VersionedRootSignatureDeserializer> m_Deserializer;
        ComPtr<ID3DBlob> m_Blob;
    };
}
