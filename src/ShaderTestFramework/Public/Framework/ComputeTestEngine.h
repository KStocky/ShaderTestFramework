#pragma once

#include "D3D12/CommandEngine.h"
#include "D3D12/DescriptorHeap.h"
#include "D3D12/GPUDevice.h"
#include "D3D12/GPUResource.h"
#include "D3D12/Shader/PipelineState.h"
#include "D3D12/Shader/RootSignature.h"

class ComputeTestEngine
{
public:

private:

    DescriptorHeap m_ResourceHeap;
    GPUResource m_AssertBuffer;
    GPUResource m_ReadbackBuffer;
    CommandEngine m_CommandEngine;
    RootSignature m_RootSig;

};
