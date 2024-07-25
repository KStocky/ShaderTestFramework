#pragma once

#include <D3D12/Shader/VirtualShaderDirectoryMapping.h>
#include <filesystem>

inline VirtualShaderDirectoryMapping GetTestVirtualDirectoryMapping()
{
    namespace fs = std::filesystem;
    fs::path shaderDir = fs::current_path();
    shaderDir += "/";
    shaderDir += SHADER_SRC;
    shaderDir += "/HLSLFrameworkTests/";

    return VirtualShaderDirectoryMapping{ "/Tests", std::move(shaderDir) };
}
