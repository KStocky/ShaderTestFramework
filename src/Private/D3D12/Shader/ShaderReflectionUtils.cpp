
#include "D3D12/Shader/ShaderReflectionUtils.h"

namespace stf
{
    bool IsOrContainsArray(ID3D12ShaderReflectionType& InType)
    {
        D3D12_SHADER_TYPE_DESC typeDesc{};
        InType.GetDesc(&typeDesc);

        if (typeDesc.Elements > 0u)
        {
            return true;
        }

        if (typeDesc.Class == D3D_SVC_STRUCT)
        {
            for (u32 memberIndex = 0; memberIndex < typeDesc.Members; ++memberIndex)
            {
                if (IsOrContainsArray(*InType.GetMemberTypeByIndex(memberIndex)))
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool ConstantBufferCanBeBoundToRootConstants(ID3D12ShaderReflectionConstantBuffer& InBuffer)
    {
        D3D12_SHADER_BUFFER_DESC bufferDesc;
        ThrowIfFailed(InBuffer.GetDesc(&bufferDesc));

        if (bufferDesc.Size > 64)
        {
            return false;
        }

        for (u32 varIndex = 0; varIndex < bufferDesc.Variables; ++varIndex)
        {
            const auto varParam = InBuffer.GetVariableByIndex(varIndex);
            D3D12_SHADER_VARIABLE_DESC varDesc{};
            ThrowIfFailed(varParam->GetDesc(&varDesc));
            
            if (IsOrContainsArray(*varParam->GetType()))
            {
                return false;
            }
        }

        return true;
    }
}