#pragma once

#include <d3d12shader.h>


namespace stf
{
    bool IsOrContainsArray(ID3D12ShaderReflectionType& InType);

    bool ConstantBufferCanBeBoundToRootConstants(ID3D12ShaderReflectionConstantBuffer& InBuffer); 
}