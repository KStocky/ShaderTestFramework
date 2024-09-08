#pragma once
#include <D3D12/Shader/ShaderCompiler.h>

namespace ShaderCompilerTestsCommon
{
    inline stf::ShaderCompiler CreateCompiler()
    {
        stf::fs::path shaderDir = stf::fs::current_path();
        shaderDir += "/";
        shaderDir += SHADER_SRC;
        shaderDir += "/D3D12/Shader/ShaderCompilerTests";

        return stf::ShaderCompiler{ std::vector{ stf::VirtualShaderDirectoryMapping{ "/Tests", std::move(shaderDir) } } };
    }

    inline stf::ShaderCompilationJobDesc CreateCompilationJob(const stf::EShaderType InType, const D3D_SHADER_MODEL InModel, const stf::EHLSLVersion InVersion, std::vector<std::wstring>&& InFlags, stf::fs::path&& InPath)
    {
        stf::ShaderCompilationJobDesc job;
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