#include "Framework/BasicShaderTestFixture.h"

#include "D3D12/GPUDevice.h"

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

    StatSystem BasicShaderTestFixtureBase::statSystem;
    std::vector<TimedStat> BasicShaderTestFixtureBase::cachedStats;

    BasicShaderTestFixtureBase::BasicShaderTestFixtureBase(const FixtureDesc& InParams)
        : m_Device(Object::New<GPUDevice>(InParams.GPUDeviceParams))
        , m_Compiler(CreateShaderCompiler(InParams.Mappings))
    {
        cachedStats.clear();
    }

    BasicShaderTestFixtureBase::~BasicShaderTestFixtureBase() noexcept
    {
        cachedStats = statSystem.FlushTimedStats();
    }

    std::vector<TimedStat> BasicShaderTestFixtureBase::GetTestStats()
    {
        return cachedStats;
    }

    ExpectedError<CompiledShaderData> BasicShaderTestFixtureBase::CompileShader(const std::string_view InName, const EShaderType InType, ShaderCompilationEnvDesc InCompileDesc, const bool InTakingCapture) const
    {
        ScopedDuration scope(std::format("BasicShaderTestFixtureBase::CompileShader: {}", InName));
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

    bool BasicShaderTestFixtureBase::ShouldTakeCapture(const EGPUCaptureMode InCaptureMode, const bool InIsFailureRetry) const
    {
        const bool takeCaptureIfAble = InCaptureMode == EGPUCaptureMode::On || (InIsFailureRetry && InCaptureMode == EGPUCaptureMode::CaptureOnFailure);
        return m_Device->IsGPUCaptureEnabled() && takeCaptureIfAble;
    }
}
