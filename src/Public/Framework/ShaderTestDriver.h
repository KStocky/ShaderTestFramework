#pragma once

#include "D3D12/CommandEngine.h"
#include "D3D12/GPUDevice.h"

#include "D3D12/Shader/PipelineState.h"
#include "D3D12/Shader/RootSignature.h"
#include "Framework/ShaderTestCommon.h"
#include "Framework/TestDataBufferLayout.h"
#include "Framework/TypeByteReader.h"

#include "Utility/Expected.h"
#include "Utility/HLSLTypes.h"
#include "Utility/Object.h"
#include "Utility/Pointer.h"

#include <dxcapi.h>
#include <vector>


namespace stf
{
    class ShaderTestDriver 
        : public Object
    {
    public:

        struct CreationParams
        {
            SharedPtr<GPUDevice> Device;
        };

        struct TestDesc
        {
            SharedPtr<Shader> Shader;
            const TestDataBufferLayout& TestBufferLayout;
            std::vector<ShaderBinding> Bindings;
            std::string_view TestName;
            uint3 DispatchConfig;
        };

        ShaderTestDriver(ObjectToken, CreationParams InParams);

        TypeReaderIndex RegisterByteReader(std::string InTypeIDName, MultiTypeByteReader InByteReader);
        TypeReaderIndex RegisterByteReader(std::string InTypeIDName, SingleTypeByteReader InByteReader);

        ExpectedError<Results> RunShaderTest(TestDesc&& InTestDesc);

    private:

        SharedPtr<PipelineState> CreatePipelineState(const RootSignature& InRootSig, IDxcBlob* InShader) const;
        Results ReadbackResults(const GPUResource& InAllocationBuffer, const GPUResource& InAssertBuffer, const uint3 InDispatchDimensions, const TestDataBufferLayout& InTestDataLayout) const;

        SharedPtr<GPUDevice> m_Device;
        SharedPtr<CommandEngine> m_CommandEngine;

        MultiTypeByteReaderMap m_ByteReaderMap;
    };
}