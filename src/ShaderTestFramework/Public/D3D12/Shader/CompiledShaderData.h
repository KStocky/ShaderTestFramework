#pragma once
#include "Utility/Pointer.h"

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
		std::string Hash = "";
	};

	CompiledShaderData() = default;
	CompiledShaderData(ShaderCompilerToken, CreationParams InParams);

	template<typename ThisType>
	auto&& GetCompiledShader(this ThisType&& InThis)
	{
		return std::forward<ThisType>(InThis).m_CompiledShader.Get();
	}

	template<typename ThisType>
	auto&& GetHash(this ThisType&& InThis)
	{
		return std::string_view{ std::forward<ThisType>(InThis).m_Hash };
	}

private:

	ComPtr<IDxcBlob> m_CompiledShader = nullptr;
	std::string m_Hash;

};