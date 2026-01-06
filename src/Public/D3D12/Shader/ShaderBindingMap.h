#pragma once

#include "D3D12/CommandEngine.h"
#include "D3D12/GPUDevice.h"
#include "D3D12/Shader/RootSignature.h"
#include "D3D12/Shader/ShaderBinding.h"
#include "Utility/Error.h"
#include "Utility/Pointer.h"
#include "Utility/TransparentStringHash.h"

#include <string>
#include <string_view>
#include <unordered_map>

#include <d3d12shader.h>

namespace stf
{
    namespace Errors
    {
        Error OnlyConstantBuffersSupportedForBinding();
        Error ConstantBufferCantBeInRootConstants(const std::string_view InBufferName);
        Error RootSignatureDWORDLimitReached();

        Error BindingIsSmallerThanBindingData(const std::string_view InBindingName, const u32 InConstantBufferSize, const u64 InBindingDataSize);
        Error BindingDoesNotExist(const std::string_view InBindingName);
    }

    class ShaderBindingMap
    {
    public:

        static ExpectedError<ShaderBindingMap> Make(ID3D12ShaderReflection& InReflection, GPUDevice& InDevice);

        const RootSignature& GetRootSig() const;

        ExpectedError<void> StageBindingData(const ShaderBinding& InBinding);
        void CommitBindings(ScopedCommandContext& InContext) const;

    private:

        struct BindingInfo
        {
            u32 RootParamIndex = 0;
            u32 OffsetIntoBuffer = 0;
            u32 BindingSize = 0;
        };

        using BindingMapType = std::unordered_map<std::string, BindingInfo, TransparentStringHash, std::equal_to<>>;
        using StagingBufferMap = std::unordered_map<u32, std::vector<u32>>;

        ShaderBindingMap(
            SharedPtr<RootSignature>&& InRootSignature,
            BindingMapType&& InNameToBindingsMap,
            StagingBufferMap&& InStagingBufferMap);

        SharedPtr<RootSignature> m_RootSignature;
        BindingMapType m_NameToBindingInfo;
        StagingBufferMap m_RootParamBuffers;
    };
}