#include "D3D12/Shader/CompiledShaderData.h"

CompiledShaderData::CompiledShaderData(ShaderCompilerToken, CreationParams InParams)
	: m_CompiledShader(std::move(InParams.CompiledShader))
	, m_Hash(std::move(InParams.Hash))
{
}
