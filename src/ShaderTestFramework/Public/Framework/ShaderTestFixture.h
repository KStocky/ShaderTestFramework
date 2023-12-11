#pragma once

#include "D3D12/CommandEngine.h"
#include "D3D12/GPUDevice.h"
#include "D3D12/Shader/ShaderCompiler.h"

#include <D3D12/GPUResource.h>
#include <vector>

class ShaderTestFixture
{
public:

    struct FailedAssertParams
    {
        u32 NumRecordedFailedAsserts = 0;
        u32 NumBytesAssertData = 0;
    };

    struct Desc
    {
        std::vector<VirtualShaderDirectoryMapping> Mappings;
        GPUDevice::CreationParams GPUDeviceParams = {
            .DebugLevel = GPUDevice::EDebugLevel::DebugLayerWithValidation,
            .DeviceType = GPUDevice::EDeviceType::Software
        };
        ShaderCodeSource Source;
        std::vector<std::wstring> CompilationFlags;
        D3D_SHADER_MODEL ShaderModel = D3D_SHADER_MODEL_6_6;
        EHLSLVersion HLSLVersion = EHLSLVersion::Default;
        FailedAssertParams AssertInfo;
    };

    struct HLSLAssertMetaData
    {
        u32 LineNumber = 0;
        u32 ThreadId = 0;
        u32 ThreadIdType = 0;
        u32 TypeId = 0;
        u32 DataAddress = 0;
        u32 DataSize = 0;
    };

    class Results
    {
    public:

        Results(std::vector<std::string> InErrors);

        template<typename ThisType>
        operator bool(this ThisType&& InThis)
        {
            return InThis.m_Errors.empty();
        }

        friend std::ostream& operator<<(std::ostream& InOs, const Results& In);

    private:

        std::vector<std::string> m_Errors;
    };

    ShaderTestFixture(Desc InParams);

    void TakeCapture();
    Results RunTest(const std::string_view InName, u32 InX, u32 InY, u32 InZ);

    bool IsValid() const;

    bool IsUsingAgilitySDK() const;

private:

    CompilationResult CompileShader(const std::string_view InName) const;
    CommandEngine CreateCommandEngine() const;
    DescriptorHeap CreateDescriptorHeap() const;
    PipelineState CreatePipelineState(const RootSignature& InRootSig, IDxcBlob* InShader) const;
    RootSignature CreateRootSignature(const CompiledShaderData& InShaderData) const;
    GPUResource CreateAssertBuffer(const u64 InSizeInBytes) const;
    GPUResource CreateReadbackBuffer(const u64 InSizeInBytes) const;
    DescriptorHandle CreateAssertBufferUAV(const GPUResource& InAssertBuffer, const DescriptorHeap& InHeap) const;
    Tuple<u32, u32> ReadbackResults(const GPUResource& InReadbackBuffer) const;
    std::vector<ShaderMacro> GenerateTypeIDDefines() const;

    u64 CalculateAssertBufferSize() const;

    bool ShouldTakeCapture() const;

    GPUDevice m_Device;
    ShaderCompiler m_Compiler;
    ShaderCodeSource m_Source;
    std::vector<std::wstring> m_CompilationFlags;
    D3D_SHADER_MODEL m_ShaderModel;
    EHLSLVersion m_HLSLVersion;
    FailedAssertParams m_AssertInfo;
    bool m_IsWarp = false;
    bool m_CaptureRequested = false;
    bool m_PIXAvailable = false;
};
