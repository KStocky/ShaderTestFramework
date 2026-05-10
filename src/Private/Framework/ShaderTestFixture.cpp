#include "Framework/ShaderTestFixture.h"

#include "D3D12/GPUDevice.h"
#include "D3D12/Shader/Shader.h"

#include "Framework/PIXCapturer.h"
#include "Utility/EnumReflection.h"

#include <format>
#include <utility>

namespace stf
{
    namespace
    {
        ShaderCompiler CreateShaderCompiler(std::vector<VirtualShaderDirectoryMapping> InMappings)
        {
            fs::path shaderDir = std::filesystem::current_path();
            shaderDir += "/";
            shaderDir += SHADER_SRC;
            InMappings.push_back({ "/Test", std::move(shaderDir) });

            return ShaderCompiler{ std::move(InMappings) };
        }
    }

    StatSystem ShaderTestFixtureBase::statSystem;
    std::vector<TimedStat> ShaderTestFixtureBase::cachedStats;

    ShaderTestFixtureBase::ShaderTestFixtureBase(const FixtureDesc& InParams)
        : m_Device(Object::New<GPUDevice>(InParams.GPUDeviceParams))
        , m_Compiler(CreateShaderCompiler(InParams.Mappings))
    {
        cachedStats.clear();
    }

    ShaderTestFixtureBase::~ShaderTestFixtureBase() noexcept
    {
        cachedStats = statSystem.FlushTimedStats();
    }

    AssertionsV1::Results ShaderTestFixture::RunTest(RuntimeTestDesc InTestDesc)
    {
        ScopedDuration fullTest(std::format("ShaderTestFixture::RunTest: {}", InTestDesc.TestName));

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

    AssertionsV1::Results ShaderTestFixture::RunCompileTimeTest(ShaderCompileTestDesc InTestDesc)
    {
        ScopedDuration scope(std::format("ShaderTestFixture::RunCompileTimeTest: {}", InTestDesc.TestName));

        auto interfaceArgs = m_Interface.GetAdditionalCompilerArgs();
        InTestDesc.CompilationEnv.CompilationFlags.insert(
            InTestDesc.CompilationEnv.CompilationFlags.end(),
            std::make_move_iterator(interfaceArgs.begin()),
            std::make_move_iterator(interfaceArgs.end()));

        return CompileShader("", EShaderType::Lib, std::move(InTestDesc.CompilationEnv), false)
            .transform(
                [](CompiledShaderData)
                {
                    return AssertionsV1::Results{ AssertionsV1::TestRunResults{} };
                })
            .or_else(
                [](Error InError) -> Expected<AssertionsV1::Results, std::monostate>
                {
                    return AssertionsV1::Results{ std::move(InError) };
                }
            ).value();
    }

    std::vector<TimedStat> ShaderTestFixtureBase::GetTestStats()
    {
        return cachedStats;
    }

    AssertionsV1::Results ShaderTestFixture::RunTestImpl(RuntimeTestDesc InTestDesc, const bool InIsFailureRetry)
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
                    const AssertionsV1::TestDataBufferLayout layout{ InTestDesc.PerTestData };
                    return m_TestDriver.RunShaderTest(
                        {
                            .Shader = InShader,
                            .PerTestData = layout,
                            .Bindings = std::move(InTestDesc.Bindings),
                            .TestName = InTestDesc.TestName,
                            .DispatchConfig = InTestDesc.ThreadGroupCount
                        }, m_Interface);
                })
            .transform(
                [](AssertionsV1::TestRunResults&& InResults) -> AssertionsV1::Results
                {
                    return AssertionsV1::Results{ std::move(InResults) };
                })
            .or_else(
                [](Error&& InError) -> Expected<AssertionsV1::Results, std::monostate>
                {
                    return AssertionsV1::Results{ std::move(InError) };
                }
            ).value();
    }

    ExpectedError<CompiledShaderData> ShaderTestFixtureBase::CompileShader(const std::string_view InName, const EShaderType InType, ShaderCompilationEnvDesc InCompileDesc, const bool InTakingCapture) const
    {
        ScopedDuration scope(std::format("ShaderTestFixture::CompileShader: {}", InName));
        ShaderCompilationJobDesc job;
        job.AdditionalFlags = std::move(InCompileDesc.CompilationFlags);
        job.AdditionalFlags.emplace_back(L"-enable-16bit-types");
        job.AdditionalFlags.emplace_back(L"-Wno-c++14-extensions");
        job.AdditionalFlags.emplace_back(L"-Wno-c++1z-extensions");
        job.EntryPoint = InName;
        job.ShaderModel = InCompileDesc.ShaderModel;
        job.ShaderType = InType;
        job.Source = std::move(InCompileDesc.Source);
        job.HLSLVersion = InCompileDesc.HLSLVersion;
        job.Defines = std::move(InCompileDesc.Defines);

        if (InTakingCapture)
        {
            job.AdditionalFlags.emplace_back(L"-Qembed_debug");
            job.AdditionalFlags.emplace_back(L"-Zss");
            job.AdditionalFlags.emplace_back(L"-Zi");
            job.Flags = Enum::MakeFlags(EShaderCompileFlags::SkipOptimization, EShaderCompileFlags::O0);
        }

        return m_Compiler.CompileShader(job);
    }

    bool ShaderTestFixtureBase::ShouldTakeCapture(const EGPUCaptureMode InCaptureMode, const bool InIsFailureRetry) const
    {
        const bool takeCaptureIfAble = InCaptureMode == EGPUCaptureMode::On || (InIsFailureRetry && InCaptureMode == EGPUCaptureMode::CaptureOnFailure);
        return m_Device->IsGPUCaptureEnabled() && takeCaptureIfAble;
    }
}
