#pragma once

#include "ShaderEnums.h"

#include <combaseapi.h>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include <d3d12.h>

namespace fs = std::filesystem;

class ShaderCodeSource
{
public:

    ShaderCodeSource() = default;
    ShaderCodeSource(std::string InSourceCode);
    ShaderCodeSource(fs::path InSourcePath);

    operator std::string() const;
    std::string ToString() const;

private:
    std::variant<std::monostate, std::string, fs::path> m_Source;
};

struct ShaderMacro
{
    std::string Name;
    std::string Definition;
};

struct ShaderCompilationJobDesc
{
    ShaderCodeSource Source;
    std::string Name;
    std::vector<ShaderMacro> Defines;
    std::vector<std::wstring> AdditionalFlags;
    std::string EntryPoint;
    D3D_SHADER_MODEL ShaderModel;
    EShaderType ShaderType;
    EShaderCompileFlags Flags;
};

std::vector<std::string> CompileShader(const ShaderCompilationJobDesc& InJob);
