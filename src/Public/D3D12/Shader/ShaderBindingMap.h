#pragma once
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

        Error ConstantBufferMustBeBoundToDecriptorTable(const std::string_view InBufferName);

        Error BindingIsSmallerThanBindingData(const std::string_view InBindingName, const u32 InConstantBufferSize, const u64 InBindingDataSize);
        Error BindingDoesNotExist(const std::string_view InBindingName);
    }

    template<typename T, typename StagingInfoType>
    concept StagingBufferFunctionType = requires(T InFunc, u32 InRootParamIndex, StagingInfoType InStagingInfo)
    {
        { InFunc(InRootParamIndex, InStagingInfo) } -> std::same_as<void>;
    };

    class ShaderBindingMap
    {
    public:

        enum class EBindType
        {
            RootConstants,
            RootDescriptor,
            DescriptorTable
        };


        struct StagingInfo
        {
            std::vector<std::byte> Buffer;
            EBindType Type = EBindType::RootConstants;
        };

        using StagingBufferMap = std::unordered_map<u32, StagingInfo>;

        static ExpectedError<ShaderBindingMap> Make(ID3D12ShaderReflection& InReflection, GPUDevice& InDevice);

        const RootSignature& GetRootSig() const;

        ExpectedError<void> StageBindingData(const ShaderBinding& InBinding);

        template<typename T>
            requires StagingBufferFunctionType<T, StagingInfo>
        void ForEachStagingBuffer(T&& InFunc)
        {
            for (const auto& [rootParamIndex, stagingInfo] : m_RootParamBuffers)
            {
                InFunc(rootParamIndex, stagingInfo);
            }
        }

    private:

        struct BindingInfo
        {
            u32 RootParamIndex = 0;
            u32 OffsetIntoBuffer = 0;
            u32 BindingSize = 0;
            EBindType Type = EBindType::RootConstants;
        };

        using BindingMapType = std::unordered_map<std::string, BindingInfo, TransparentStringHash, std::equal_to<>>;

        ShaderBindingMap(
            SharedPtr<RootSignature>&& InRootSignature,
            BindingMapType&& InNameToBindingsMap,
            StagingBufferMap&& InStagingBufferMap);

        SharedPtr<RootSignature> m_RootSignature;
        BindingMapType m_NameToBindingInfo;
        StagingBufferMap m_RootParamBuffers;
    };
}