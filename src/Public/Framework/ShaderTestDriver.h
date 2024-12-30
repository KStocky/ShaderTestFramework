#pragma once

#include "D3D12/CommandEngine.h"
#include "D3D12/GPUDevice.h"

#include "D3D12/DescriptorRingAllocator.h"
#include "D3D12/Shader/PipelineState.h"
#include "D3D12/Shader/RootSignature.h"
#include "Framework/HLSLTypes.h"
#include "Framework/ShaderTestShader.h"
#include "Framework/TestDataBufferLayout.h"
#include "Framework/TypeByteReader.h"

#include "Utility/Object.h"
#include "Utility/Pointer.h"

#include <dxcapi.h>
#include <string_view>
#include <vector>


namespace stf
{
    class ShaderTestDriver : Object
    {
    public:

        struct CreationParams
        {
            SharedPtr<GPUDevice> Device;
        };

        struct UAV
        {
            SharedPtr<GPUResource> Resource;
            u32 BindlessIndex = 0;
        };

        ShaderTestDriver(CreationParams InParams);

        SharedPtr<GPUResource> CreateBuffer(const D3D12_HEAP_TYPE InType, const D3D12_RESOURCE_DESC1& InDesc);
        UAV CreateUAV(SharedPtr<GPUResource> InResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc);

        u32 RegisterByteReader(std::string InTypeIDName, MultiTypeByteReader InByteReader);
        u32 RegisterByteReader(std::string InTypeIDName, SingleTypeByteReader InByteReader);

        Results RunShaderTest(const ShaderTestShader& InShader, const TestDataBufferLayout& InTestBufferLayout, const std::string_view InTestName, const uint3 InDispatchConfig);

    private:

        struct DescriptorHeapAndAllocator
        {
            SharedPtr<DescriptorHeap> CPUHeapHeap;
            DescriptorRingAllocator Allocator;
        };

        SharedPtr<DescriptorHeap> CreateDescriptorHeap() const;
        SharedPtr<PipelineState> CreatePipelineState(const RootSignature& InRootSig, IDxcBlob* InShader) const;

        void ResizeDescriptorHeap(const u32 InNewSize);

        Results ReadbackResults(const GPUResource& InAllocationBuffer, const GPUResource& InAssertBuffer, const uint3 InDispatchDimensions, const TestDataBufferLayout& InTestDataLayout) const;

        SharedPtr<GPUDevice> m_Device;
        SharedPtr<CommandEngine> m_CommandEngine;

        DescriptorHeapAndAllocator m_DescriptorAllocator;
        MultiTypeByteReaderMap m_ByteReaderMap;
    };
}