#pragma once
#include <D3D12/Shader/VirtualShaderDirectoryMapping.h>
#include <Framework/ShaderTestFixture.h>
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

class ShaderTestFixtureBaseFixture
{
public:

    ShaderTestFixtureBaseFixture()
        : ShaderTestFixtureBaseFixture(
            ShaderTestFixture::FixtureDesc
            {
                .Mappings{ GetTestVirtualDirectoryMapping() }
            }
        )
    {}

    ShaderTestFixtureBaseFixture(ShaderTestFixture::FixtureDesc InDesc)
        : fixture(std::move(InDesc))
    {}

protected:
    mutable ShaderTestFixture fixture;
};
