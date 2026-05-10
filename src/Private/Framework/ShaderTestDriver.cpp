
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

    ExpectedError<AssertionsV1::TestRunResults> ShaderTestDriver::RunShaderTest(TestDesc&& InTestDesc, AssertionsV1::AssertionsV1Interface& InInterface)
    {
        auto pipelineState = CreatePipelineState(InTestDesc.Shader->GetRootSig(), InTestDesc.Shader->GetCompiledShader());

        using GPUResources = AssertionsV1::AssertionsV1Interface::GPUResourcesType;
        using GPUReadbacks = AssertionsV1::AssertionsV1Interface::GPUReadbackResourcesType;

        return m_CommandEngine->Execute(InTestDesc.TestName,
            [&](ScopedCommandContext& InContext) -> ExpectedError<GPUReadbacks>
            {
                return InContext.Section("Test Setup",
                    [&](ScopedCommandContext& InContext) -> ExpectedError<GPUResources>
                    {
                        InContext->SetPipelineState(*pipelineState);
                        return InInterface.CreateGPUResources(InContext, InTestDesc.PerTestData);
                    })
                    .and_then(
                        [&](GPUResources&& InResources) -> ExpectedError<GPUResources>
                        {
                            return InContext.Section("Test Dispatch",
                                [&](ScopedCommandContext& InContext)
                                {
                                    return InContext.Dispatch(InTestDesc.DispatchConfig, InTestDesc.Shader,
                                        [&](ScopedCommandShader& InShader) -> ExpectedError<void>
                                        {
                                            if (auto result = InInterface.BindShaderData(InShader, InResources, InTestDesc.PerTestData); !result)
                                            {
                                                return result;
                                            }

                                            for (const auto& binding : InTestDesc.Bindings)
                                            {
                                                if (auto result = InShader.StageBindingData(binding); !result)
                                                {
                                                    return result;
                                                }
                                            }

                                            return {};
                                        });
                                })
                                .transform(
                                    [&]() -> GPUResources
                                    {
                                        return InResources;
                                    });
                        })
                    .and_then(
                        [&](const GPUResources& InResources)
                        {
                            return InContext.Section("Results readback",
                                [&](ScopedCommandContext& InContext)
                                {
                                    return InInterface.QueueReadbacks(InContext, InResources);
                                });
                        });
            })
            .and_then(
                [&](const GPUReadbacks& InReadbacks)
                {
                    m_CommandEngine->Flush();
                    return InInterface.ProcessReadbacks(*m_CommandEngine, InReadbacks, InTestDesc.PerTestData);
                });
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
