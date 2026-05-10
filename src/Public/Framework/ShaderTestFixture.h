#pragma once

#include "D3D12/GPUDevice.h"
#include "D3D12/Shader/ShaderBinding.h"
#include "D3D12/Shader/ShaderCompiler.h"
#include "Framework/ShaderTestDriver.h"
#include "Framework/AssertionsV1/AssertionsV1Interface.h"
#include "Framework/AssertionsV1/Results.h"
#include "Framework/AssertionsV1/TestDataBufferLayout.h"
#include "Stats/StatSystem.h"
#include "Utility/Error.h"
#include "Utility/HLSLTypes.h"
#include "Utility/Pointer.h"
#include "Utility/TransparentStringHash.h"
#include <vector>

namespace stf
{

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

    struct ShaderCompilationEnvDesc
    {
        ShaderCodeSource Source;
        std::vector<std::wstring> CompilationFlags{};
        std::vector<ShaderMacro> Defines{};
        D3D_SHADER_MODEL ShaderModel = D3D_SHADER_MODEL_6_6;
        EHLSLVersion HLSLVersion = EHLSLVersion::v202x;
    };

    struct ShaderCompileTestDesc
    {
        ShaderCompilationEnvDesc CompilationEnv;
        std::string_view TestName;
    };

    class ShaderTestFixtureBase
    {

    public:
        struct FixtureDesc
        {
            std::vector<VirtualShaderDirectoryMapping> Mappings;
            GPUDevice::CreationParams GPUDeviceParams
            {
                .DebugLevel = GPUDevice::EDebugLevel::DebugLayer,
                .DeviceType = GPUDevice::EDeviceType::Software,
                .EnableGPUCapture = false
            };
        };

        ShaderTestFixtureBase(const FixtureDesc& InParams);
        ~ShaderTestFixtureBase() noexcept;

        static std::vector<TimedStat> GetTestStats();

    protected:

        static StatSystem statSystem;
        static constexpr auto StatSystemGetter = []() -> StatSystem& { return statSystem; };
        using ScopedDuration = ScopedCPUDurationStat<StatSystemGetter>;
        static std::vector<TimedStat> cachedStats;

        ExpectedError<CompiledShaderData> CompileShader(const std::string_view InName, const EShaderType InType, ShaderCompilationEnvDesc InCompileDesc, const bool InTakingCapture) const;

        bool ShouldTakeCapture(const EGPUCaptureMode InCaptureMode, const bool InIsFailureRetry) const;

        SharedPtr<GPUDevice> m_Device;
        ShaderCompiler m_Compiler;
    };

    class ShaderTestFixture
        : public ShaderTestFixtureBase
    {
    public:
        
        ShaderTestFixture(const FixtureDesc& InParams, AssertionsV1::AssertionsV1Interface InInterface = {})
            : ShaderTestFixtureBase{ InParams }
            , m_Interface{ std::move(InInterface) }
            , m_TestDriver{ 
                ShaderTestDriver::CreationParams
                {
                    .Device = m_Device
                } }
        {
        }

        struct RuntimeTestDesc
        {
            ShaderCompilationEnvDesc CompilationEnv;
            std::string_view TestName;
            std::vector<ShaderBinding> Bindings {};
            uint3 ThreadGroupCount{};
            AssertionsV1::TestDataBufferLayoutDesc PerTestData
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

        AssertionsV1::Results RunTest(RuntimeTestDesc InTestDesc);
        AssertionsV1::Results RunCompileTimeTest(ShaderCompileTestDesc InTestDesc);

    private:
        AssertionsV1::Results RunTestImpl(RuntimeTestDesc InTestDesc, const bool InIsFailureRetry);

        AssertionsV1::AssertionsV1Interface m_Interface;
        ShaderTestDriver m_TestDriver;
    };
}
