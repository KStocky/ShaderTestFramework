#pragma once

#include "Utility/MoveOnly.h"
#include "Platform.h"
#include "Utility/Pointer.h"

#include <d3d12.h>

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

	template<typename ThisType>
	D3D12_ROOT_SIGNATURE_DESC1 GetDesc(this ThisType&& InThis)
	{
		return std::forward<ThisType>(InThis).m_Deserializer->GetUnconvertedRootSignatureDesc()->Desc_1_1;
	}

	template<typename ThisType>
	ID3D12RootSignature* GetRaw(this ThisType&& InThis)
	{
		return std::forward<ThisType>(InThis).m_RootSig.Get();
	}

	template<typename ThisType>
	operator ID3D12RootSignature* (this ThisType&& InThis)
	{
		return std::forward<ThisType>(InThis).GetRaw();
	}

private:

	ComPtr<ID3D12RootSignature> m_RootSig;
	ComPtr<ID3D12VersionedRootSignatureDeserializer> m_Deserializer;
	ComPtr<ID3DBlob> m_Blob;
};
