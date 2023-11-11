#include "Framework/ShaderTestFixture.h"

#include "D3D12/CommandEngine.h"
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

	auto Engine = [this]()
	{
		auto CommandList = m_Device.CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
		D3D12_COMMAND_QUEUE_DESC queueDesc;
		queueDesc.NodeMask = 0;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		auto CommandQueue = m_Device.CreateCommandQueue(queueDesc);

		return CommandEngine(CommandEngine::CreationParams{ std::move(CommandList), std::move(CommandQueue), m_Device });
	}();

	auto ResourceHeap = [this]()
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = 0;
		desc.NumDescriptors = 1;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		return m_Device.CreateDescriptorHeap(desc);
	}();

	auto RootSignature = m_Device.CreateRootSignature(compileResult.value());

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
