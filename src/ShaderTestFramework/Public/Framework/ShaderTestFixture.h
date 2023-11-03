#pragma once

#include "D3D12/GPUDevice.h"
#include "D3D12/Shader/ShaderCompiler.h"

class ShaderTestFixture
{
public:

    ShaderTestFixture();

    bool IsValid() const;

    bool IsUsingAgilitySDK() const;

private:

    GPUDevice m_Device;
    ShaderCompiler m_Compiler;
};
