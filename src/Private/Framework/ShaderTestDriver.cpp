
#include "Framework/ShaderTestDriver.h"

#include <d3dx12/d3dx12.h>

namespace stf
{
    ShaderTestDriver::ShaderTestDriver(CreationParams InParams)
        : m_Device(std::move(InParams.Device))
        , m_CommandEngine(Object::New<CommandEngine>(
            CommandEngine::CreationParams
            {
                .Device = m_Device
            }
        ))
    {
    }

    SharedPtr<PipelineState> ShaderTestDriver::CreatePipelineState(const RootSignature& InRootSig, IDxcBlob* InShader) const
    {
        return m_Device->CreatePipelineState(
            D3D12_COMPUTE_PIPELINE_STATE_DESC
            {
                .pRootSignature = InRootSig,
                .CS
                {
                    .pShaderBytecode = InShader->GetBufferPointer(),
                    .BytecodeLength = InShader->GetBufferSize()
                },
                .NodeMask = 0,
                .CachedPSO
                {
                    .pCachedBlob = nullptr,
                    .CachedBlobSizeInBytes = 0
                },
                .Flags = D3D12_PIPELINE_STATE_FLAG_NONE
            });
    }
}
