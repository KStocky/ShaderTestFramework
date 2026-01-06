#pragma once

#include "D3D12/Shader/CompiledShaderData.h"
#include "D3D12/Shader/ShaderEnums.h"
#include "D3D12/Shader/VirtualShaderDirectoryMappingManager.h"
#include "Utility/Error.h"

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

    namespace Errors
    {
        Error EmptyShaderCodeSource();
        Error EmptyVirtualPath();
        Error UnknownVirtualShaderMappingError();
        Error ResolvedPathIsInvalid(const std::string_view InAbsolutePath, const std::string_view InPath);
        Error ReportShaderCompilationError(const std::string_view InError);
    }

    class ShaderCodeSource
    {
    public:

        ShaderCodeSource() = default;
        ShaderCodeSource(std::string InSourceCode);
        ShaderCodeSource(fs::path InSourcePath);

        ExpectedError<std::string> ToString(const VirtualShaderDirectoryMappingManager& InManager) const;

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
        ExpectedError<CompiledShaderData> CompileShader(const ShaderCompilationJobDesc& InJob) const;

    private:

        void Init();

        VirtualShaderDirectoryMappingManager m_DirectoryManager;
        ComPtr<IDxcUtils> m_Utils = nullptr;
        ComPtr<IDxcCompiler3> m_Compiler = nullptr;
        ComPtr<IDxcIncludeHandler> m_IncludeHandler = nullptr;
    };
}
