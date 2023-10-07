#pragma once

#include "Pointer.h"

class GPUDevice;

class ShaderTestFixture
{
public:

    ShaderTestFixture();
    ~ShaderTestFixture();

    bool IsValid() const;

    bool IsUsingAgilitySDK() const;

private:

    UniquePtr<GPUDevice> m_Device;
};
