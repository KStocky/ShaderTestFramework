#pragma once

#include "D3D12/GPUDevice.h"

class ShaderTestFixture
{
public:

    ShaderTestFixture();

    bool IsValid() const;

    bool IsUsingAgilitySDK() const;

private:

    UniquePtr<GPUDevice> m_Device;
};
