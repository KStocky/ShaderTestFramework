#include "Framework/ShaderTestFixture.h"

#include "D3D12/CommandEngine.h"
#include "D3D12/GPUDevice.h"

#include <format>
#include <ranges>

#include <d3dx12/d3dx12.h>

ShaderTestFixture::ShaderTestFixture(Desc InParams)
	: m_Device(InParams.GPUDeviceParams)
	, m_Compiler(std::move(InParams.Mappings))
	, m_Source(std::move(InParams.Source))
	, m_CompilationFlags(std::move(InParams.CompilationFlags))
	, m_ShaderModel(InParams.ShaderModel)
	, m_IsWarp(InParams.GPUDeviceParams.DeviceType == GPUDevice::EDeviceType::Software)
{
}

ShaderTestFixture::Results ShaderTestFixture::RunTest(std::string InName, u32 InX, u32 InY, u32 InZ)
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

	auto engine = [this]()
	{
		auto commandList = m_Device.CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
		D3D12_COMMAND_QUEUE_DESC queueDesc;
		queueDesc.NodeMask = 0;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		auto commandQueue = m_Device.CreateCommandQueue(queueDesc);

		return CommandEngine(CommandEngine::CreationParams{ std::move(commandList), std::move(commandQueue), m_Device });
	}();

	auto resourceHeap = [this]()
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = 0;
		desc.NumDescriptors = 1;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		return m_Device.CreateDescriptorHeap(desc);
	}();

	auto rootSignature = m_Device.CreateRootSignature(compileResult.value());

	auto pipelineState = [this, &compileResult, &rootSignature]()
	{
		D3D12_COMPUTE_PIPELINE_STATE_DESC desc;
		desc.CachedPSO.CachedBlobSizeInBytes = 0;
		desc.CachedPSO.pCachedBlob = nullptr;
		desc.CS.BytecodeLength = compileResult->GetCompiledShader()->GetBufferSize();
		desc.CS.pShaderBytecode = compileResult->GetCompiledShader()->GetBufferPointer();
		desc.Flags = m_IsWarp ? D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG : D3D12_PIPELINE_STATE_FLAG_NONE;
		desc.NodeMask = 0;
		desc.pRootSignature = rootSignature;
		return m_Device.CreatePipelineState(desc);
	}();

	static constexpr u64 bufferSizeInBytes = 10'000'000ull;
	auto assertBuffer = [this]()
	{
		const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		const auto resourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(bufferSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		return m_Device.CreateCommittedResource(heapProps, D3D12_HEAP_FLAG_NONE, resourceDesc, D3D12_BARRIER_LAYOUT_UNDEFINED);
	}();

	auto readBackBuffer = [this]()
	{
		const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
		const auto resourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(bufferSizeInBytes);
		return m_Device.CreateCommittedResource(heapProps, D3D12_HEAP_FLAG_NONE, resourceDesc, D3D12_BARRIER_LAYOUT_UNDEFINED);
	}();

	const auto assertUAV = [&resourceHeap, &assertBuffer, this]()
	{
		const auto uav = ThrowIfUnexpected(resourceHeap.CreateDescriptorHandle(0));
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Buffer.CounterOffsetInBytes = 0;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
		uavDesc.Buffer.NumElements = bufferSizeInBytes / 4;
		uavDesc.Buffer.StructureByteStride = 0;
		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		m_Device.CreateUnorderedAccessView(assertBuffer, uavDesc, uav);
		return uav;
	}();

	engine.Execute(
		Lambda
		(
			[](DescriptorHeap& InHeap, PipelineState& InPipelineState, RootSignature& InRootSig, const u32& InX, const u32& InY, const u32& InZ, ScopedCommandContext& InContext)
			{
				InContext->SetPipelineState(InPipelineState);
				InContext->SetGraphicsRootSignature(InRootSig);
				InContext->SetDescriptorHeaps(InHeap);
				InContext->Dispatch(InX, InY, InZ);
			},
			&resourceHeap,
			&pipelineState,
			&rootSignature,
			InX,
			InY,
			InZ
		)
	);

	engine.Flush();

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
