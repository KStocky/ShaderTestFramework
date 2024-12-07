#pragma once

#include "D3D12/CommandEngine.h"
#include "D3D12/GPUDevice.h"
#include "D3D12/GPUResource.h"
#include "D3D12/Shader/ShaderBinding.h"
#include "D3D12/Shader/ShaderCompiler.h"
#include "Framework/HLSLTypes.h"
#include "Framework/TestDataBufferLayout.h"
#include "Framework/TestDataBufferProcessor.h"
#include "Stats/StatSystem.h"
#include "Utility/Expected.h"
#include "Utility/Pointer.h"
#include "Utility/TransparentStringHash.h"
#include <optional>
#include <vector>

namespace stf
{
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
            std::vector<ShaderBinding> Bindings;
            uint3 ThreadGroupCount{};
            TestDataBufferLayoutDesc TestDataLayout
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

        Results RunTest(RuntimeTestDesc InTestDesc);
        Results RunCompileTimeTest(CompileTestDesc InTestDesc);
        void RegisterByteReader(std::string InTypeIDName, MultiTypeByteReader InByteReader);
        void RegisterByteReader(std::string InTypeIDName, SingleTypeByteReader InByteReader);

        bool IsValid() const;

        bool IsUsingAgilitySDK() const;

        static std::vector<TimedStat> GetTestStats();

    private:

        static StatSystem statSystem;
        static constexpr auto StatSystemGetter = []() -> StatSystem& { return statSystem; };
        using ScopedDuration = ScopedCPUDurationStat<StatSystemGetter>;
        static std::vector<TimedStat> cachedStats;

        struct BindingInfo
        {
            u32 RootParamIndex = 0;
            u32 OffsetIntoBuffer = 0;
            u32 BindingSize = 0;
        };

        struct ReflectionResults
        {
            RootSignature RootSig;
            std::unordered_map<std::string, BindingInfo, TransparentStringHash, std::equal_to<>> NameToBindingInfo;
            std::unordered_map<u32, std::vector<u32>> RootParamBuffers;
        };

        Results RunTestImpl(RuntimeTestDesc InTestDesc, const bool InIsFailureRetry);

        CompilationResult CompileShader(const std::string_view InName, const EShaderType InType, CompilationEnvDesc InCompileDesc, const bool InTakingCapture) const;
        CommandEngine CreateCommandEngine() const;
        SharedPtr<DescriptorHeap> CreateDescriptorHeap() const;
        SharedPtr<PipelineState> CreatePipelineState(const RootSignature& InRootSig, IDxcBlob* InShader) const;
        Expected<ReflectionResults, ErrorTypeAndDescription> ProcessShaderReflection(const CompiledShaderData& InShaderData) const;
        Expected<void, ErrorTypeAndDescription> PopulateTestConstantBuffers(ReflectionResults& InOutReflectionResults, const std::span<const ShaderBinding> InBindings) const;
        SharedPtr<GPUResource> CreateAssertBuffer(const u64 InSizeInBytes) const;
        SharedPtr<GPUResource> CreateReadbackBuffer(const u64 InSizeInBytes) const;
        DescriptorHandle CreateAssertBufferUAV(const GPUResource& InAssertBuffer, const DescriptorHeap& InHeap, const u32 InIndex) const;
        Results ReadbackResults(const GPUResource& InAllocationBuffer, const GPUResource& InAssertBuffer, const uint3 InDispatchDimensions, const TestDataBufferLayout& InTestDataLayout) const;
        void PopulateDefaultByteReaders();

        bool ShouldTakeCapture(const EGPUCaptureMode InCaptureMode, const bool InIsFailureRetry) const;

        SharedPtr<GPUDevice> m_Device;
        ShaderCompiler m_Compiler;
        MultiTypeByteReaderMap m_ByteReaderMap;
        std::vector<ShaderMacro> m_Defines;
    };
}
