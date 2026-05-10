#pragma once

#include "D3D12/CommandEngine.h"
#include "D3D12/GPUDevice.h"

#include "D3D12/Shader/PipelineState.h"
#include "D3D12/Shader/RootSignature.h"

#include "Framework/AssertionsV1/AssertionsV1Interface.h"
#include "Framework/AssertionsV1/Results.h"
#include "Framework/AssertionsV1/TestDataBufferLayout.h"

#include "Utility/Expected.h"
#include "Utility/HLSLTypes.h"
#include "Utility/MoveOnly.h"
#include "Utility/Pointer.h"

#include <dxcapi.h>
#include <vector>


namespace stf
{
    class ShaderTestDriver 
        : MoveOnly
    {
    public:

        struct CreationParams
        {
            SharedPtr<GPUDevice> Device;
        };

        struct TestDesc
        {
            SharedPtr<Shader> Shader;
            const AssertionsV1::TestDataBufferLayout& PerTestData;
            std::vector<ShaderBinding> Bindings;
            std::string_view TestName;
            uint3 DispatchConfig;
        };

        ShaderTestDriver(CreationParams InParams);

        ExpectedError<AssertionsV1::TestRunResults> RunShaderTest(TestDesc&& InTestDesc, AssertionsV1::AssertionsV1Interface& InInterface);

    private:

        SharedPtr<PipelineState> CreatePipelineState(const RootSignature& InRootSig, IDxcBlob* InShader) const;

        SharedPtr<GPUDevice> m_Device;
        SharedPtr<CommandEngine> m_CommandEngine;
    };
}