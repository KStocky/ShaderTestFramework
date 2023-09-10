#pragma once

#include <d3d12.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class ShaderTestFixture
{
public:

    ShaderTestFixture();

    bool IsValid() const;
private:

    ComPtr<ID3D12Device> m_Device = nullptr;
};
