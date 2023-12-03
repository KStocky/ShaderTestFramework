#include "Framework/ShaderTestFixture.h"

#include "Framework/PIXCapturer.h"
#include "Utility/EnumReflection.h"

#include "D3D12/CommandEngine.h"
#include "D3D12/GPUDevice.h"

#include <format>
#include <ranges>

#include <WinPixEventRuntime/pix3.h>

ShaderTestFixture::ShaderTestFixture(Desc InParams)
	: m_Device()
	, m_Compiler(std::move(InParams.Mappings))
	, m_Source(std::move(InParams.Source))
	, m_CompilationFlags(std::move(InParams.CompilationFlags))
	, m_ShaderModel(InParams.ShaderModel)
    , m_HLSLVersion(InParams.HLSLVersion)
	, m_IsWarp(InParams.GPUDeviceParams.DeviceType == GPUDevice::EDeviceType::Software)
{
    m_PIXAvailable = PIXLoadLatestWinPixGpuCapturerLibrary() != nullptr;
    m_Device = GPUDevice{ InParams.GPUDeviceParams };
}

void ShaderTestFixture::TakeCapture()
{
    m_CaptureRequested = true;
}

ShaderTestFixture::Results ShaderTestFixture::RunTest(const std::string_view InName, u32 InX, u32 InY, u32 InZ)
{
	const auto compileResult = CompileShader(InName);

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

    const auto capturer = PIXCapturer(InName, ShouldTakeCapture());

    engine.Execute(InName,
        [&resourceHeap,
        &pipelineState,
        &rootSignature,
        &assertBuffer,
        &readBackBuffer,
        InX, InY, InZ]
        (ScopedCommandContext& InContext)
        {
            InContext.Section("Test Setup",
                [&](ScopedCommandContext& InContext)
                {
                    InContext->SetPipelineState(pipelineState);
                    InContext->SetComputeRootSignature(rootSignature);
                    InContext->SetDescriptorHeaps(resourceHeap);
                    InContext->SetBufferUAV(assertBuffer);
                    std::array params{ 0u };
                    InContext->SetComputeRoot32BitConstants(0, std::span{ params }, 0);
                }
            );

            InContext.Section("Test Dispatch",
                [InX, InY, InZ](ScopedCommandContext& InContext)
                {
                    InContext->Dispatch(InX, InY, InZ);
                }
            );

            InContext.Section("Results readback",
                [&](ScopedCommandContext& InContext)
                {
                    InContext->CopyBufferResource(readBackBuffer, assertBuffer);
                }
            );
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

CompilationResult ShaderTestFixture::CompileShader(const std::string_view InName) const
{
    ShaderCompilationJobDesc job;
    job.AdditionalFlags = m_CompilationFlags;
    job.EntryPoint = InName;
    job.ShaderModel = m_ShaderModel;
    job.ShaderType = EShaderType::Compute;
    job.Source = m_Source;
    job.HLSLVersion = m_HLSLVersion;

    if (ShouldTakeCapture())
    {
        job.AdditionalFlags.emplace_back(L"-Qembed_debug");
        job.AdditionalFlags.emplace_back(L"-Zss");
        job.AdditionalFlags.emplace_back(L"-Zi");
        job.Flags = Enum::MakeFlags(EShaderCompileFlags::SkipOptimization, EShaderCompileFlags::O0);
    }

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
    desc.NumDescriptors = 2;
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

bool ShaderTestFixture::ShouldTakeCapture() const
{
    return m_PIXAvailable && m_CaptureRequested;
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
