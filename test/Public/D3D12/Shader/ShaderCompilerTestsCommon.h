#pragma once
#include <D3D12/Shader/ShaderCompiler.h>

namespace ShaderCompilerTestsCommon
{
    inline ShaderCompiler CreateCompiler()
    {
        fs::path shaderDir = fs::current_path();
        shaderDir += "/";
        shaderDir += SHADER_SRC;
        shaderDir += "/D3D12/Shader/ShaderCompilerTests";

        return ShaderCompiler{ std::vector{ VirtualShaderDirectoryMapping{ "/Tests", std::move(shaderDir) } } };
    }

    inline ShaderCompilationJobDesc CreateCompilationJob(const EShaderType InType, const D3D_SHADER_MODEL InModel, const EHLSLVersion InVersion, std::vector<std::wstring>&& InFlags, fs::path&& InPath)
    {
        ShaderCompilationJobDesc job;
        job.Name = "Test";
        job.EntryPoint = "Main";
        job.ShaderModel = InModel;
        job.ShaderType = InType;
        job.AdditionalFlags = std::move(InFlags);
        job.Source = std::move(InPath);
        job.HLSLVersion = InVersion;

        return job;
    }
}