#include "D3D12/Shader/RootSignature.h"

RootSignature::RootSignature(CreationParams InParams)
	: m_RootSig(std::move(InParams.RootSig))
	, m_Deserializer(std::move(InParams.Deserializer))
	, m_Blob(std::move(InParams.Blob))
{
}
