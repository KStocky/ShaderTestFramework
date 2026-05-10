#pragma once

#include "D3D12/GPUDevice.h"
#include "D3D12/Shader/Shader.h"
#include "D3D12/Shader/ShaderBinding.h"
#include "D3D12/Shader/ShaderCompiler.h"
#include "Framework/AssertionInterface.h"
#include "Framework/AssertionInterface/Results.h"
#include "Framework/PIXCapturer.h"
#include "Framework/ShaderTestDriver.h"
#include "Stats/StatSystem.h"
#include "Utility/Error.h"
#include "Utility/Expected.h"
#include "Utility/HLSLTypes.h"
#include "Utility/Pointer.h"
#include "Utility/TransparentStringHash.h"

#include <format>
#include <iterator>
#include <string>
#include <utility>
#include <variant>
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

    class BasicShaderTestFixtureBase
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

        BasicShaderTestFixtureBase(const FixtureDesc& InParams);
        ~BasicShaderTestFixtureBase() noexcept;

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

    template<assert::CAssertionInterfaceType TInterface>
    class BasicShaderTestFixture
        : public BasicShaderTestFixtureBase
    {
    public:

        using InterfaceType = TInterface;
        using ResultsType = assert::Results<typename TInterface::TestRunResultsType>;

        BasicShaderTestFixture(const FixtureDesc& InParams, TInterface InInterface = {})
            : BasicShaderTestFixtureBase{ InParams }
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
            typename TInterface::PerTestData PerTestData {};
            EStringMaxLength StringMaxLength = EStringMaxLength::s64;
            EStringMode StringMode = EStringMode::OnFailure;
            EGPUCaptureMode GPUCaptureMode = EGPUCaptureMode::Off;
        };

        ResultsType RunTest(RuntimeTestDesc InTestDesc)
        {
            ScopedDuration fullTest(std::format("BasicShaderTestFixture::RunTest: {}", InTestDesc.TestName));

            InTestDesc.CompilationEnv.Defines.push_back(
                ShaderMacro
                {
                    .Name = "TTL_STRING_MAX_LENGTH",
                    .Definition = std::to_string(static_cast<i32>(InTestDesc.StringMaxLength))
                });

            auto interfaceArgs = m_Interface.GetAdditionalCompilerArgs();
            InTestDesc.CompilationEnv.CompilationFlags.insert(
                InTestDesc.CompilationEnv.CompilationFlags.end(),
                std::make_move_iterator(interfaceArgs.begin()),
                std::make_move_iterator(interfaceArgs.end()));

            const bool requestedRetryOnFail =
                InTestDesc.GPUCaptureMode == EGPUCaptureMode::CaptureOnFailure ||
                InTestDesc.StringMode == EStringMode::OnFailure;

            if (!requestedRetryOnFail)
            {
                return RunTestImpl(std::move(InTestDesc), false);
            }

            if (auto firstResult = RunTestImpl(InTestDesc, false))
            {
                return firstResult;
            }
            else if (firstResult.GetTestRunError())
            {
                return firstResult;
            }

            return RunTestImpl(std::move(InTestDesc), true);
        }

        ResultsType RunCompileTimeTest(ShaderCompileTestDesc InTestDesc)
        {
            ScopedDuration scope(std::format("BasicShaderTestFixture::RunCompileTimeTest: {}", InTestDesc.TestName));

            auto interfaceArgs = m_Interface.GetAdditionalCompilerArgs();
            InTestDesc.CompilationEnv.CompilationFlags.insert(
                InTestDesc.CompilationEnv.CompilationFlags.end(),
                std::make_move_iterator(interfaceArgs.begin()),
                std::make_move_iterator(interfaceArgs.end()));

            return CompileShader("", EShaderType::Lib, std::move(InTestDesc.CompilationEnv), false)
                .transform(
                    [](CompiledShaderData)
                    {
                        return ResultsType{ typename TInterface::TestRunResultsType{} };
                    })
                .or_else(
                    [](Error InError) -> Expected<ResultsType, std::monostate>
                    {
                        return ResultsType{ std::move(InError) };
                    }
                ).value();
        }

    private:
        ResultsType RunTestImpl(RuntimeTestDesc InTestDesc, const bool InIsFailureRetry)
        {
            const bool takeCapture = ShouldTakeCapture(InTestDesc.GPUCaptureMode, InIsFailureRetry);
            const bool enableStrings = InTestDesc.StringMode == EStringMode::On || (InIsFailureRetry && InTestDesc.StringMode == EStringMode::OnFailure);
            InTestDesc.CompilationEnv.Defines.push_back(
                ShaderMacro
                {
                    .Name = "TTL_ENABLE_STRINGS",
                    .Definition = enableStrings ? "1" : "0"
                }
            );

            return CompileShader(InTestDesc.TestName, EShaderType::Compute, std::move(InTestDesc.CompilationEnv), takeCapture)
                .and_then(
                    [&](const CompiledShaderData& InCompilationResult)
                    {
                        return Shader::Make(InCompilationResult, *m_Device);
                    })
                .and_then(
                    [&](const SharedPtr<Shader>& InShader)
                    {
                        const auto capturer = PIXCapturer(InTestDesc.TestName, takeCapture);
                        return m_TestDriver.RunShaderTest(
                            ShaderTestDriver::TestDesc<TInterface>
                            {
                                .Shader = InShader,
                                .PerTestData = InTestDesc.PerTestData,
                                .Bindings = std::move(InTestDesc.Bindings),
                                .TestName = InTestDesc.TestName,
                                .DispatchConfig = InTestDesc.ThreadGroupCount
                            }, m_Interface);
                    })
                .transform(
                    [](typename TInterface::TestRunResultsType&& InResults) -> ResultsType
                    {
                        return ResultsType{ std::move(InResults) };
                    })
                .or_else(
                    [](Error&& InError) -> Expected<ResultsType, std::monostate>
                    {
                        return ResultsType{ std::move(InError) };
                    }
                ).value();
        }

        TInterface m_Interface;
        ShaderTestDriver m_TestDriver;
    };
}
