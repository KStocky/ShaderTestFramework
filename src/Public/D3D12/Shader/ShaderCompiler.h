#pragma once

#include "D3D12/Shader/CompiledShaderData.h"
#include "D3D12/Shader/ShaderEnums.h"
#include "D3D12/Shader/VirtualShaderDirectoryMappingManager.h"
#include "Utility/Expected.h"

#include <filesystem>
#include <ostream>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <d3d12.h>

namespace stf
{
    namespace fs = std::filesystem;

    using CompilationResult = Expected<CompiledShaderData, std::string>;

    std::ostream& operator<<(std::ostream& InStream, const CompilationResult& InResult);

    class ShaderCodeSource
    {
    public:

        ShaderCodeSource() = default;
        ShaderCodeSource(std::string InSourceCode);
        ShaderCodeSource(fs::path InSourcePath);

        std::string ToString(const VirtualShaderDirectoryMappingManager& InManager) const;

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
        std::string_view Name;
        std::vector<ShaderMacro> Defines;
        std::vector<std::wstring> AdditionalFlags;
        std::string EntryPoint;
        D3D_SHADER_MODEL ShaderModel = D3D_SHADER_MODEL_6_0;
        EShaderType ShaderType = EShaderType::None;
        EHLSLVersion HLSLVersion = EHLSLVersion::Default;
        EShaderCompileFlags Flags = EShaderCompileFlags::None;
    };

    class ShaderCompiler
    {
    public:

        ShaderCompiler();
        ShaderCompiler(std::vector<VirtualShaderDirectoryMapping> InMappings);
        CompilationResult CompileShader(const ShaderCompilationJobDesc& InJob) const;

    private:

        void Init();

        VirtualShaderDirectoryMappingManager m_DirectoryManager;
        ComPtr<IDxcUtils> m_Utils = nullptr;
        ComPtr<IDxcCompiler3> m_Compiler = nullptr;
        ComPtr<IDxcIncludeHandler> m_IncludeHandler = nullptr;
    };
}
