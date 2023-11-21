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
	, m_IsWarp(InParams.GPUDeviceParams.DeviceType == GPUDevice::EDeviceType::Software)
{
}

ShaderTestFixture::Results ShaderTestFixture::RunTest(std::string InName, u32 InX, u32 InY, u32 InZ)
{
	const auto compileResult = CompileShader(std::move(InName));

	if (!compileResult.has_value())
	{
		return Results{ {compileResult.error()} };
	}

    auto engine = CreateCommandEngine();
    auto resourceHeap = CreateDescriptorHeap();
	auto rootSignature = CreateRootSignature(compileResult.value());
    auto pipelineState = CreatePipelineState(rootSignature, compileResult->GetCompiledShader());

	static constexpr u64 bufferSizeInBytes = 8ull;
    auto assertBuffer = CreateAssertBuffer(bufferSizeInBytes);
    auto readBackBuffer = CreateReadbackBuffer(bufferSizeInBytes);

    const auto assertUAV = CreateAssertBufferUAV(assertBuffer, resourceHeap);

    engine.Execute(
        [&resourceHeap, 
        &pipelineState, 
        &rootSignature, 
        &assertBuffer, 
        &readBackBuffer, 
        InX, 
        InY, 
        InZ]
        (ScopedCommandContext& InContext)
        {
            InContext->SetPipelineState(pipelineState);
            InContext->SetComputeRootSignature(rootSignature);
            InContext->SetDescriptorHeaps(resourceHeap);
            InContext->SetBufferUAV(assertBuffer);
            std::array params{ 10u, 0u };
            InContext->SetComputeRoot32BitConstants(0, std::span{ params }, 0);
            InContext->Dispatch(InX, InY, InZ);
            InContext->CopyBufferResource(readBackBuffer, assertBuffer);
        }
    );

	engine.Flush();

    const auto [numSuccessAsserts, numFailedAsserts] = ReadbackResults(readBackBuffer);

	if (numFailedAsserts != 0)
	{
		return Results({ std::format("There were {} successful asserts and {} failed assertions", numSuccessAsserts, numFailedAsserts) });
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

CompilationResult ShaderTestFixture::CompileShader(std::string InName) const
{
    ShaderCompilationJobDesc job;
    job.AdditionalFlags = m_CompilationFlags;
    job.EntryPoint = std::move(InName);
    job.ShaderModel = m_ShaderModel;
    job.ShaderType = EShaderType::Compute;
    job.Source = m_Source;

    return m_Compiler.CompileShader(job);
}

CommandEngine ShaderTestFixture::CreateCommandEngine() const
{
    auto commandList = m_Device.CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
    D3D12_COMMAND_QUEUE_DESC queueDesc;
    queueDesc.NodeMask = 0;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    auto commandQueue = m_Device.CreateCommandQueue(queueDesc);

    return CommandEngine(CommandEngine::CreationParams{ std::move(commandList), std::move(commandQueue), m_Device });
}

DescriptorHeap ShaderTestFixture::CreateDescriptorHeap() const
{
    D3D12_DESCRIPTOR_HEAP_DESC desc;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    desc.NodeMask = 0;
    desc.NumDescriptors = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    return m_Device.CreateDescriptorHeap(desc);
}

PipelineState ShaderTestFixture::CreatePipelineState(const RootSignature& InRootSig, IDxcBlob* InShader) const
{
    D3D12_COMPUTE_PIPELINE_STATE_DESC desc;
    desc.CachedPSO.CachedBlobSizeInBytes = 0;
    desc.CachedPSO.pCachedBlob = nullptr;
    desc.CS.BytecodeLength = InShader->GetBufferSize();
    desc.CS.pShaderBytecode = InShader->GetBufferPointer();
    desc.Flags = m_IsWarp ? D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG : D3D12_PIPELINE_STATE_FLAG_NONE;
    desc.NodeMask = 0;
    desc.pRootSignature = InRootSig;
    return m_Device.CreatePipelineState(desc);
}

RootSignature ShaderTestFixture::CreateRootSignature(const CompiledShaderData& InShaderData) const
{
    return m_Device.CreateRootSignature(InShaderData);
}

GPUResource ShaderTestFixture::CreateAssertBuffer(const u64 InSizeInBytes) const
{
    const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    const auto resourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(InSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

    return m_Device.CreateCommittedResource(heapProps, D3D12_HEAP_FLAG_NONE, resourceDesc, D3D12_BARRIER_LAYOUT_UNDEFINED);
}

GPUResource ShaderTestFixture::CreateReadbackBuffer(const u64 InSizeInBytes) const
{
    const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
    const auto resourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(InSizeInBytes);
    return m_Device.CreateCommittedResource(heapProps, D3D12_HEAP_FLAG_NONE, resourceDesc, D3D12_BARRIER_LAYOUT_UNDEFINED);
}

DescriptorHandle ShaderTestFixture::CreateAssertBufferUAV(const GPUResource& InAssertBuffer, const DescriptorHeap& InHeap) const
{
    const auto uav = ThrowIfUnexpected(InHeap.CreateDescriptorHandle(0));
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.Buffer.CounterOffsetInBytes = 0;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
    uavDesc.Buffer.NumElements = static_cast<u32>(InAssertBuffer.GetDesc().Width) / 4;
    uavDesc.Buffer.StructureByteStride = 0;
    uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    m_Device.CreateUnorderedAccessView(InAssertBuffer, uavDesc, uav);
    return uav;
}

Tuple<u32, u32> ShaderTestFixture::ReadbackResults(const GPUResource& InReadbackBuffer) const
{
    const auto mappedReadbackData = InReadbackBuffer.Map();
    const auto assertData = mappedReadbackData.Get();

    u32 success = 0;
    u32 fails = 0;

    std::memcpy(&success, assertData.data(), sizeof(u32));
    std::memcpy(&fails, assertData.data() + sizeof(u32), sizeof(u32));

    return Tuple{ success, fails };
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
