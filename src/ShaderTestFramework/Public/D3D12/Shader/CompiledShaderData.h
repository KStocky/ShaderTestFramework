#pragma once
#include "D3D12/Shader/ShaderHash.h"
#include "Utility/Pointer.h"

#include <optional>
#include <string>

#include <dxcapi.h>

class ShaderCompilerToken
{
	friend class ShaderCompiler;
	ShaderCompilerToken() = default;
};

class CompiledShaderData
{
public:
	struct CreationParams
	{
		ComPtr<IDxcBlob> CompiledShader = nullptr;
		std::optional<ShaderHash> Hash = {};
	};

	CompiledShaderData() = default;
	CompiledShaderData(ShaderCompilerToken, CreationParams InParams);

	template<typename ThisType>
	decltype(auto) GetCompiledShader(this ThisType&& InThis)
	{
		return std::forward<ThisType>(InThis).m_CompiledShader.Get();
	}

	template<typename ThisType>
	decltype(auto) GetShaderHash(this ThisType&& InThis)
	{
		return std::forward<ThisType>(InThis).m_Hash;
	}

private:

	ComPtr<IDxcBlob> m_CompiledShader = nullptr;
	std::optional<ShaderHash> m_Hash;
};