#include "Framework/ShaderTestFixture.h"

#include "Framework/AssertBufferProcessor.h"
#include "Framework/HLSLTypes.h"
#include "Framework/PIXCapturer.h"
#include "Utility/EnumReflection.h"
#include "Utility/Math.h"

#include "D3D12/CommandEngine.h"
#include "D3D12/GPUDevice.h"

#include <format>
#include <ranges>

#include <WinPixEventRuntime/pix3.h>

ShaderTestFixture::ShaderTestFixture(Desc InParams)
	: m_Device()
	, m_Compiler()
	, m_Source(std::move(InParams.Source))
	, m_CompilationFlags(std::move(InParams.CompilationFlags))
    , m_Defines(std::move(InParams.Defines))
	, m_ShaderModel(InParams.ShaderModel)
    , m_HLSLVersion(InParams.HLSLVersion)
    , m_AssertInfo(InParams.AssertInfo)
	, m_IsWarp(InParams.GPUDeviceParams.DeviceType == GPUDevice::EDeviceType::Software)
{
    fs::path shaderDir = std::filesystem::current_path();
    shaderDir += "/";
    shaderDir += SHADER_SRC;
    InParams.Mappings.push_back({ "/Test", std::move(shaderDir) });
    m_Compiler.emplace(std::move(InParams.Mappings));
    m_PIXAvailable = PIXLoadLatestWinPixGpuCapturerLibrary() != nullptr;
    m_Device = GPUDevice{ InParams.GPUDeviceParams };

    PopulateDefaultByteReaders();
}

void ShaderTestFixture::TakeCapture()
{
    m_CaptureRequested = true;
}

STF::Results ShaderTestFixture::RunTest(const std::string_view InName, u32 InX, u32 InY, u32 InZ)
{
	const auto compileResult = CompileShader(InName, EShaderType::Compute);

	if (!compileResult.has_value())
	{
		return STF::Results{ {compileResult.error()} };
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
                        m_AssertInfo.NumFailedAsserts, m_AssertInfo.NumBytesAssertData, static_cast<u32>(bufferSizeInBytes), 1u
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

    return ReadbackResults(readBackAllocationBuffer, readBackBuffer, uint3(dimX, dimY, dimZ));
}

STF::Results ShaderTestFixture::RunCompileTimeTest()
{
    const auto compileResult = CompileShader("", EShaderType::Lib);

    if (!compileResult.has_value())
    {
        return STF::Results{ {compileResult.error()} };
    }

    return STF::Results{ STF::TestRunResults{} };
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

CompilationResult ShaderTestFixture::CompileShader(const std::string_view InName, const EShaderType InType) const
{
    ShaderCompilationJobDesc job;
    job.AdditionalFlags = m_CompilationFlags;
    job.AdditionalFlags.emplace_back(L"-enable-16bit-types");
    job.EntryPoint = InName;
    job.ShaderModel = m_ShaderModel;
    job.ShaderType = InType;
    job.Source = m_Source;
    job.HLSLVersion = m_HLSLVersion;
    job.Defines = m_Defines;

    if (ShouldTakeCapture())
    {
        job.AdditionalFlags.emplace_back(L"-Qembed_debug");
        job.AdditionalFlags.emplace_back(L"-Zss");
        job.AdditionalFlags.emplace_back(L"-Zi");
        job.Flags = Enum::MakeFlags(EShaderCompileFlags::SkipOptimization, EShaderCompileFlags::O0);
    }

    return m_Compiler->CompileShader(job);
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

void ShaderTestFixture::RegisterByteReader(std::string InTypeIDName, STF::MultiTypeByteReader InByteReader)
{
    const u32 typeId = m_NextTypeID++;
    ThrowIfFalse(typeId == static_cast<u32>(m_ByteReaderMap.size()));
    m_Defines.push_back(ShaderMacro{ std::move(InTypeIDName), std::format("{}", typeId) });
    m_ByteReaderMap.push_back(std::move(InByteReader));
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

STF::Results ShaderTestFixture::ReadbackResults(const GPUResource& InAllocationBuffer, const GPUResource& InAssertBuffer, const uint3 InDispatchDimensions) const
{
    const auto mappedAllocationData = InAllocationBuffer.Map();
    const auto allocationData = mappedAllocationData.Get();

    u32 success = 0;
    u32 fails = 0;

    std::memcpy(&success, allocationData.data(), sizeof(u32));
    std::memcpy(&fails, allocationData.data() + sizeof(u32), sizeof(u32));


    const auto mappedAssertData = InAssertBuffer.Map();
    const auto assertData = mappedAssertData.Get();

    return STF::ProcessAssertBuffer(success, fails, InDispatchDimensions, m_AssertInfo, assertData, m_ByteReaderMap);
}

void ShaderTestFixture::PopulateDefaultByteReaders()
{
    std::array typeIdDefines =
    {
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_UNDEFINED", [](const u16, const std::span<const std::byte> InBytes) -> std::string
        { 
            return std::format("Undefined Type -> {}", STF::DefaultByteReader(0, InBytes)); 
        } },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_BOOL", [](const u16, const std::span<const std::byte> InBytes) -> std::string
        { 
            if (InBytes.size_bytes() != 4)
            {
                return std::format("Unexpected num bytes: {} for type bool", InBytes.size_bytes());
            }

            u32 data;
            std::memcpy(&data, InBytes.data(), InBytes.size_bytes());

            return std::format("{}", data != 0 ? "true" : "false");
        } },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_BOOL2", STF::CreateMultiTypeByteReader<bool2>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_BOOL3", STF::CreateMultiTypeByteReader<bool3>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_BOOL4", STF::CreateMultiTypeByteReader<bool4>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_INT16", STF::CreateMultiTypeByteReader<i16>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_INT16_2", STF::CreateMultiTypeByteReader<int16_t2>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_INT16_3", STF::CreateMultiTypeByteReader<int16_t3>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_INT16_4", STF::CreateMultiTypeByteReader<int16_t4>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_INT", STF::CreateMultiTypeByteReader<i32>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_INT2", STF::CreateMultiTypeByteReader<int2>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_INT3", STF::CreateMultiTypeByteReader<int3>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_INT4", STF::CreateMultiTypeByteReader<int4>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_INT64", STF::CreateMultiTypeByteReader<i64>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_INT64_2", STF::CreateMultiTypeByteReader<int64_t2>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_INT64_3", STF::CreateMultiTypeByteReader<int64_t3>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_INT64_4", STF::CreateMultiTypeByteReader<int64_t4>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_UINT16", STF::CreateMultiTypeByteReader<u16>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_UINT16_2", STF::CreateMultiTypeByteReader<uint16_t2>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_UINT16_3", STF::CreateMultiTypeByteReader<uint16_t3>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_UINT16_4", STF::CreateMultiTypeByteReader<uint16_t4>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_UINT", STF::CreateMultiTypeByteReader<u32>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_UINT2", STF::CreateMultiTypeByteReader<uint2>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_UINT3", STF::CreateMultiTypeByteReader<uint3>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_UINT4", STF::CreateMultiTypeByteReader<uint4>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_UINT64", STF::CreateMultiTypeByteReader<u64>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_UINT64_2", STF::CreateMultiTypeByteReader<uint64_t2>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_UINT64_3", STF::CreateMultiTypeByteReader<uint64_t3>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_UINT64_4", STF::CreateMultiTypeByteReader<uint64_t4>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_FLOAT16", STF::CreateMultiTypeByteReader<f16>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_FLOAT16_2", STF::CreateMultiTypeByteReader<float16_t2>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_FLOAT16_3", STF::CreateMultiTypeByteReader<float16_t3>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_FLOAT16_4", STF::CreateMultiTypeByteReader<float16_t4>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_FLOAT", STF::CreateMultiTypeByteReader<float>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_FLOAT2", STF::CreateMultiTypeByteReader<float2>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_FLOAT3", STF::CreateMultiTypeByteReader<float3>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_FLOAT4", STF::CreateMultiTypeByteReader<float4>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_FLOAT64", STF::CreateMultiTypeByteReader<f64>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_FLOAT64_2", STF::CreateMultiTypeByteReader<float64_t2>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_FLOAT64_3", STF::CreateMultiTypeByteReader<float64_t3>() },
        Tuple<std::string, STF::MultiTypeByteReader>{"TYPE_ID_FLOAT64_4", STF::CreateMultiTypeByteReader<float64_t4>() },
    };
    
    m_Defines.reserve(typeIdDefines.size());
    m_ByteReaderMap.reserve(typeIdDefines.size());

    for (auto&& [define, converter] : typeIdDefines)
    {
        RegisterByteReader(std::move(define), std::move(converter));
    }
}

bool ShaderTestFixture::ShouldTakeCapture() const
{
    return m_PIXAvailable && m_CaptureRequested;
}

u64 ShaderTestFixture::CalculateAssertBufferSize() const
{    
    const u64 assertInfoSection = AlignedOffset(m_AssertInfo.NumFailedAsserts * sizeof(STF::HLSLAssertMetaData), 8ull);
    const u64 assertDataSection = m_AssertInfo.NumBytesAssertData > 0 ? AlignedOffset(m_AssertInfo.NumBytesAssertData, 8ull) : 0;

    const u64 requestedSize = assertInfoSection + assertDataSection;

    return requestedSize > 0 ? requestedSize : 4ull;
}
