#pragma once
#include <D3D12/Shader/VirtualShaderDirectoryMapping.h>
#include <Framework/AssertionsV1/ShaderTestFixture.h>
#include <filesystem>

inline stf::VirtualShaderDirectoryMapping GetTestVirtualDirectoryMapping()
{
    namespace fs = std::filesystem;
    fs::path shaderDir = fs::current_path();
    shaderDir += "/";
    shaderDir += SHADER_SRC;
    shaderDir += "/HLSLFrameworkTests/";

    return stf::VirtualShaderDirectoryMapping{ "/Tests", std::move(shaderDir) };
}

class ShaderTestFixtureBaseFixture
{
public:

    ShaderTestFixtureBaseFixture()
        : ShaderTestFixtureBaseFixture(
            stf::AssertionsV1::ShaderTestFixture::FixtureDesc
            {
                .Mappings{ GetTestVirtualDirectoryMapping() }
            }
        )
    {}

    ShaderTestFixtureBaseFixture(stf::AssertionsV1::ShaderTestFixture::FixtureDesc InDesc)
        : fixture(std::move(InDesc))
    {}

    ShaderTestFixtureBaseFixture(stf::AssertionsV1::ShaderTestFixture::FixtureDesc InDesc, stf::AssertionsV1::AssertionsV1Interface InInterface)
        : fixture(std::move(InDesc), std::move(InInterface))
    {}

protected:
    mutable stf::AssertionsV1::ShaderTestFixture fixture;
};
