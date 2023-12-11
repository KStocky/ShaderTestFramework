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
    , m_AssertInfo(InParams.AssertInfo)
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

    const auto [dimX, dimY, dimZ] = [&compileResult, InX, InY, InZ]()
        {
            u32 threadX = 0;
            u32 threadY = 0;
            u32 threadZ = 0;
            compileResult->GetReflection()->GetThreadGroupSize(&threadX, &threadY, &threadZ);

            const u32 dimX = threadX * InX;
            const u32 dimY = threadY * InY;
            const u32 dimZ = threadZ * InZ;

            return Tuple{ dimX, dimY, dimZ };
        }();

	const u64 bufferSizeInBytes = CalculateAssertBufferSize();
    auto assertBuffer = CreateAssertBuffer(bufferSizeInBytes);
    auto allocationBuffer = CreateAssertBuffer(12ull);
    auto readBackBuffer = CreateReadbackBuffer(bufferSizeInBytes);
    auto readBackAllocationBuffer = CreateReadbackBuffer(12ull);

    const auto assertUAV = CreateAssertBufferUAV(assertBuffer, resourceHeap, 0);
    const auto allocationUAV = CreateAssertBufferUAV(allocationBuffer, resourceHeap, 1);

    const auto capturer = PIXCapturer(InName, ShouldTakeCapture());

    engine.Execute(InName,
        [&resourceHeap,
        &pipelineState,
        &rootSignature,
        &assertBuffer,
        &allocationBuffer,
        &readBackBuffer,
        &readBackAllocationBuffer,
        InX, InY, InZ,
        dimX, dimY, dimZ,
        bufferSizeInBytes,
        this]
        (ScopedCommandContext& InContext)
        {
            InContext.Section("Test Setup",
                [&](ScopedCommandContext& InContext)
                {
                    InContext->SetPipelineState(pipelineState);
                    InContext->SetDescriptorHeaps(resourceHeap);
                    InContext->SetComputeRootSignature(rootSignature);
                    InContext->SetBufferUAV(assertBuffer);
                    InContext->SetBufferUAV(allocationBuffer);
                    std::array params
                    { 
                        0u, dimX, dimY, dimZ, 
                        m_AssertInfo.NumRecordedFailedAsserts, m_AssertInfo.NumBytesAssertData, static_cast<u32>(bufferSizeInBytes), 1u
                    };
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
                    InContext->CopyBufferResource(readBackAllocationBuffer, allocationBuffer);
                }
            );
        }
    );

	engine.Flush();

    const auto [numSuccessAsserts, numFailedAsserts] = ReadbackResults(readBackAllocationBuffer, readBackBuffer);

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
    job.Defines = GenerateTypeIDDefines();

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

DescriptorHandle ShaderTestFixture::CreateAssertBufferUAV(const GPUResource& InAssertBuffer, const DescriptorHeap& InHeap, const u32 InIndex) const
{
    const auto uav = ThrowIfUnexpected(InHeap.CreateDescriptorHandle(InIndex));
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

Tuple<u32, u32> ShaderTestFixture::ReadbackResults(const GPUResource& InAllocationBuffer, const GPUResource&) const
{
    const auto mappedReadbackData = InAllocationBuffer.Map();
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

std::vector<ShaderMacro> ShaderTestFixture::GenerateTypeIDDefines() const
{
    return std::vector<ShaderMacro>
    {
        { "TYPE_ID_BOOL", "1" },
        { "TYPE_ID_INT", "2" },
        { "TYPE_ID_INT2", "3" },
        { "TYPE_ID_INT3", "4" },
        { "TYPE_ID_INT4", "5" },
        { "TYPE_ID_UINT", "6" },
        { "TYPE_ID_UINT2", "7" },
        { "TYPE_ID_UINT3", "8" },
        { "TYPE_ID_UINT4", "9" },
        { "TYPE_ID_FLOAT", "10" },
        { "TYPE_ID_FLOAT2", "11" },
        { "TYPE_ID_FLOAT3", "12" },
        { "TYPE_ID_FLOAT4", "13" }
    };
}

u64 ShaderTestFixture::CalculateAssertBufferSize() const
{    
    auto RoundUpToMultipleOf4 = [](const u64 In)
    {
        return (In + 3ull) & ~3ull;
    };

    const u64 assertInfoSection = m_AssertInfo.NumRecordedFailedAsserts * sizeof(HLSLAssertMetaData);
    const u64 assertDataSection = m_AssertInfo.NumBytesAssertData > 0 ? RoundUpToMultipleOf4(m_AssertInfo.NumBytesAssertData) : 0;

    const u64 requestedSize = assertInfoSection + assertDataSection;

    return requestedSize > 0 ? requestedSize : 4ull;
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
