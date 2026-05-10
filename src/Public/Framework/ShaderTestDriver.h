#pragma once

#include "D3D12/CommandEngine.h"
#include "D3D12/GPUDevice.h"

#include "D3D12/Shader/PipelineState.h"
#include "D3D12/Shader/RootSignature.h"
#include "D3D12/Shader/Shader.h"
#include "D3D12/Shader/ShaderBinding.h"

#include "Framework/AssertionInterface.h"

#include "Utility/Expected.h"
#include "Utility/HLSLTypes.h"
#include "Utility/MoveOnly.h"
#include "Utility/Pointer.h"

#include <dxcapi.h>
#include <string_view>
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

        template<assert::CAssertionInterfaceType TInterface>
        struct TestDesc
        {
            SharedPtr<Shader> Shader;
            const typename TInterface::PerTestData& PerTestData;
            std::vector<ShaderBinding> Bindings;
            std::string_view TestName;
            uint3 DispatchConfig;
        };

        ShaderTestDriver(CreationParams InParams);

        template<assert::CAssertionInterfaceType TInterface>
        ExpectedError<typename TInterface::TestRunResultsType> RunShaderTest(TestDesc<TInterface>&& InTestDesc, TInterface& InInterface);

    private:

        SharedPtr<PipelineState> CreatePipelineState(const RootSignature& InRootSig, IDxcBlob* InShader) const;

        SharedPtr<GPUDevice> m_Device;
        SharedPtr<CommandEngine> m_CommandEngine;
    };

    template<assert::CAssertionInterfaceType TInterface>
    ExpectedError<typename TInterface::TestRunResultsType> ShaderTestDriver::RunShaderTest(TestDesc<TInterface>&& InTestDesc, TInterface& InInterface)
    {
        auto pipelineState = CreatePipelineState(InTestDesc.Shader->GetRootSig(), InTestDesc.Shader->GetCompiledShader());

        using GPUResources = typename TInterface::GPUResourcesType;
        using GPUReadbacks = typename TInterface::GPUReadbackResourcesType;

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
}