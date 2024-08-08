#pragma once

#include "D3D12/CommandEngine.h"
#include "D3D12/GPUDevice.h"
#include "D3D12/GPUResource.h"
#include "D3D12/Shader/ShaderCompiler.h"
#include "Framework/HLSLTypes.h"
#include "Framework/TestDataBufferLayout.h"
#include "Framework/TestDataBufferProcessor.h"
#include "Stats/StatSystem.h"
#include <optional>
#include <vector>

class ShaderTestFixture
{
public:

    enum class EGPUCaptureMode : u8
    {
        Off,
        CaptureOnFailure,
        On
    };

    enum class EStringMode : u8
    {
        Off,
        OnFailure,
        On
    };

    enum class EStringMaxLength
    {
        s16 = 16,
        s64 = 64,
        s256 = 256
    };

    struct FixtureDesc
    {
        std::vector<VirtualShaderDirectoryMapping> Mappings;
        GPUDevice::CreationParams GPUDeviceParams
        {
            .DebugLevel = GPUDevice::EDebugLevel::DebugLayerWithValidation,
            .DeviceType = GPUDevice::EDeviceType::Software,
            .EnableGPUCapture = false
        };
    };

    struct CompilationEnvDesc
    {
        ShaderCodeSource Source;
        std::vector<std::wstring> CompilationFlags;
        std::vector<ShaderMacro> Defines;
        D3D_SHADER_MODEL ShaderModel = D3D_SHADER_MODEL_6_6;
        EHLSLVersion HLSLVersion = EHLSLVersion::v202x;
    };

    struct RuntimeTestDesc
    {
        CompilationEnvDesc CompilationEnv;
        std::string_view TestName;
        uint3 ThreadGroupCount{};
        STF::TestDataBufferLayoutDesc TestDataLayout
        { 
            .NumFailedAsserts = 100u, 
            .NumBytesAssertData = 10000u, 
            .NumStrings = 100u, 
            .NumBytesStringData = 800u, 
            .NumSections = 100u 
        };
        EStringMaxLength StringMaxLength = EStringMaxLength::s64;
        EStringMode StringMode = EStringMode::OnFailure;
        EGPUCaptureMode GPUCaptureMode = EGPUCaptureMode::Off;
    };

    struct CompileTestDesc
    {
        CompilationEnvDesc CompilationEnv;
        std::string_view TestName;
    };

    ShaderTestFixture(FixtureDesc InParams);
    ~ShaderTestFixture() noexcept;

    STF::Results RunTest(RuntimeTestDesc InTestDesc);
    STF::Results RunCompileTimeTest(CompileTestDesc InTestDesc);
    void RegisterByteReader(std::string InTypeIDName, STF::MultiTypeByteReader InByteReader);
    void RegisterByteReader(std::string InTypeIDName, STF::SingleTypeByteReader InByteReader);

    bool IsValid() const;

    bool IsUsingAgilitySDK() const;

    static std::vector<TimedStat> GetTestStats();

private:

    static StatSystem statSystem;
    static constexpr auto StatSystemGetter = []() -> StatSystem& { return statSystem; };
    using ScopedDuration = ScopedCPUDurationStat<StatSystemGetter>;
    static std::vector<TimedStat> cachedStats;

    STF::Results RunTestImpl(RuntimeTestDesc InTestDesc, const bool InIsFailureRetry);

    CompilationResult CompileShader(const std::string_view InName, const EShaderType InType, CompilationEnvDesc InCompileDesc, const bool InTakingCapture) const;
    CommandEngine CreateCommandEngine() const;
    DescriptorHeap CreateDescriptorHeap() const;
    PipelineState CreatePipelineState(const RootSignature& InRootSig, IDxcBlob* InShader) const;
    RootSignature CreateRootSignature(const CompiledShaderData& InShaderData) const;
    GPUResource CreateAssertBuffer(const u64 InSizeInBytes) const;
    GPUResource CreateReadbackBuffer(const u64 InSizeInBytes) const;
    DescriptorHandle CreateAssertBufferUAV(const GPUResource& InAssertBuffer, const DescriptorHeap& InHeap, const u32 InIndex) const;
    STF::Results ReadbackResults(const GPUResource& InAllocationBuffer, const GPUResource& InAssertBuffer, const uint3 InDispatchDimensions, const STF::TestDataBufferLayout& InTestDataLayout) const;
    void PopulateDefaultByteReaders();

    bool ShouldTakeCapture(const EGPUCaptureMode InCaptureMode, const bool InIsFailureRetry) const;

    GPUDevice m_Device;
    ShaderCompiler m_Compiler;
    STF::MultiTypeByteReaderMap m_ByteReaderMap;
    std::vector<ShaderMacro> m_Defines;
};
