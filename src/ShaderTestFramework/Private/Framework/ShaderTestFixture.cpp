#include "Framework/ShaderTestFixture.h"

#include "D3D12/GPUDevice.h"

#include <format>
#include <ranges>

ShaderTestFixture::ShaderTestFixture(Desc InParams)
	: m_Device(InParams.GPUDeviceParams)
	, m_Compiler(std::move(InParams.Mappings))
	, m_Source(std::move(InParams.Source))
	, m_CompilationFlags(std::move(InParams.CompilationFlags))
	, m_ShaderModel(InParams.ShaderModel)
{
	
}

ShaderTestFixture::Results ShaderTestFixture::RunTest(std::string InName, u32, u32, u32)
{
	ShaderCompilationJobDesc job;
	job.AdditionalFlags = m_CompilationFlags;
	job.EntryPoint = std::move(InName);
	job.ShaderModel = m_ShaderModel;
	job.ShaderType = EShaderType::Compute;
	job.Source = m_Source;

	const auto compileResult = m_Compiler.CompileShader(job);

	if (!compileResult.has_value())
	{
		return Results{ {compileResult.error()} };
	}

	return Results({});
}

bool ShaderTestFixture::IsValid() const
{
    return m_Device.IsValid();
}

bool ShaderTestFixture::IsUsingAgilitySDK() const
{
	if (!IsValid())
	{
		return false;
	}

	return m_Device.GetHardwareInfo().FeatureInfo.EnhancedBarriersSupport;
}

ShaderTestFixture::Results::Results(std::vector<std::string> InErrors)
	: m_Errors(std::move(InErrors))
{
}

std::ostream& operator<<(std::ostream& InOs, const ShaderTestFixture::Results& In)
{
	for (const auto& [index, error] : std::views::enumerate(In.m_Errors))
	{
		InOs << std::format("{}. {}\n", index, error);
	}
	return InOs;
}
