#pragma once

#include "Container/RingBuffer.h"
#include "D3D12/CommandAllocator.h"
#include "D3D12/CommandQueue.h"
#include "D3D12/GPUDevice.h"
#include "D3D12/GPUResourceManager.h"
#include "D3D12/Shader/Shader.h"
#include "Utility/Error.h"

#include "Utility/FunctionTraits.h"
#include "Utility/HLSLTypes.h"
#include "Utility/Lambda.h"
#include "Utility/Object.h"
#include "Utility/Pointer.h"

#include <WinPixEventRuntime/pix3.h>

namespace stf
{
    class CommandEngineToken
    {
        friend class CommandEngine;
        CommandEngineToken() = default;
    };

    class CommandEngine;
    class ScopedCommandContext;
    class ScopedCommandShader;

    template<typename T>
    concept CommandEngineFuncType = LambdaType<T> && requires
    {
        requires T::ParamTypes::Size == 1;
        requires std::same_as<typename T::ParamTypes::template Type<0>, ScopedCommandContext&>;
        requires std::same_as<typename T::RetType, ExpectedError<void>>;
    };

    template<typename T>
    concept ExecuteLambdaType =
        !CommandEngineFuncType<T> &&
        TFuncTraits<T>::ParamTypes::Size == 1 &&
        std::is_same_v<typename TFuncTraits<T>::ParamTypes::template Type<0>, ScopedCommandContext&> &&
        std::is_same_v<typename TFuncTraits<T>::ReturnType, ExpectedError<void>>;

    template<typename T>
    concept BindShaderLambdaType = !LambdaType<T> && 
        TFuncTraits<T>::ParamTypes::Size == 2 &&
        std::is_same_v<typename TFuncTraits<T>::ParamTypes::template Type<0>, ScopedCommandContext&> &&
        std::is_same_v<typename TFuncTraits<T>::ParamTypes::template Type<1>, ScopedCommandShader&> &&
        std::is_same_v<typename TFuncTraits<T>::ReturnType, ExpectedError<void>>;

    class ScopedGPUResourceManager
    {
    public:
    
        ScopedGPUResourceManager(const SharedPtr<GPUResourceManager>& InResourceManager);
    
        ~ScopedGPUResourceManager() noexcept;
    
        [[nodiscard]] GPUResourceManager::ConstantBufferViewHandle CreateCBV(const std::span<const std::byte> InData);

        [[nodiscard]] GPUResourceManager::BufferHandle CreateBuffer(const GPUResourceManager::BufferDesc& InBufferDesc);

        [[nodiscard]] GPUResourceManager::BufferUAVHandle CreateUAV(const GPUResourceManager::BufferHandle& InBufferHandle, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc);

        ExpectedError<GPUResourceManager::ReadbackResultHandle> QueueReadback(CommandList& InList, const GPUResourceManager::BufferHandle InBufferHandle);

        void SetUAV(CommandList& InList, const GPUResourceManager::BufferUAVHandle InHandle);

        void SetRootDescriptor(CommandList& InList, const u32 InRootParamIndex, const GPUResourceManager::ConstantBufferViewHandle InHandle);

        void SetDescriptorHeap(CommandList& InList);

        ExpectedError<u32> GetDescriptorIndex(const GPUResourceManager::DescriptorOpaqueHandle InHandle) const;
    
    private:
    
        SharedPtr<GPUResourceManager> m_ResourceManager;
        std::vector<GPUResourceManager::ConstantBufferHandle> m_ConstantBuffers;
        std::vector<GPUResourceManager::ConstantBufferViewHandle> m_CBVs;
        std::vector<GPUResourceManager::BufferHandle> m_Buffers;
        std::vector<GPUResourceManager::BufferUAVHandle> m_BufferUAVs;
    };

    class CommandShaderToken
    {
        friend class ScopedCommandShader;
        CommandShaderToken() = default;
    };

    class ScopedCommandShader
    {
    public:

        ScopedCommandShader(const SharedPtr<Shader>& InShader);
        ScopedCommandShader(const ScopedCommandShader&) = delete;
        ScopedCommandShader(ScopedCommandShader&&) = delete;
        ScopedCommandShader& operator=(const ScopedCommandShader&) = delete;
        ScopedCommandShader& operator=(ScopedCommandShader&&) = delete;

        ExpectedError<void> StageBindingData(const ShaderBinding& InBinding);

        void StageBindlessResource(ScopedCommandContext& InContext, std::string InBindingName, const GPUResourceManager::BufferUAVHandle InHandle);

    private:

        SharedPtr<Shader> m_Shader;
    };

    class ScopedCommandContext
    {
    public:

        ScopedCommandContext(CommandEngineToken,
            const SharedPtr<CommandList>& InList,
            const SharedPtr<GPUResourceManager>& InResourceManager
        );

        ScopedCommandContext(const ScopedCommandContext&) = delete;
        ScopedCommandContext(ScopedCommandContext&&) = delete;
        ScopedCommandContext& operator=(const ScopedCommandContext&) = delete;
        ScopedCommandContext& operator=(ScopedCommandContext&&) = delete;

        CommandList* operator->() const;

        CommandList& operator*() const;

        template<ExecuteLambdaType InLambdaType>
        ExpectedError<void> Section(const std::string_view InName, InLambdaType&& InFunc)
        {
            PIXScopedEvent(m_List->GetRaw(), PIX_COLOR(0, 255, 0), "%s", InName.data());
            return InFunc(*this);
        }

        CommandList* GetList() const;

        [[nodiscard]] GPUResourceManager::BufferHandle CreateBuffer(const GPUResourceManager::BufferDesc& InDesc);

        [[nodiscard]] GPUResourceManager::BufferUAVHandle CreateUAV(const GPUResourceManager::BufferHandle& InBufferHandle, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc);

        void SetUAV(const GPUResourceManager::BufferUAVHandle InHandle);

        [[nodiscard]] GPUResourceManager::ConstantBufferViewHandle CreateCBV(const std::span<const std::byte> InData);

        ExpectedError<GPUResourceManager::ReadbackResultHandle> QueueReadback(const GPUResourceManager::BufferHandle InBufferHandle);

        void SetRootDescriptor(const u32 InRootParamIndex, const GPUResourceManager::ConstantBufferViewHandle InHandle);

        template<BindShaderLambdaType BindFunc>
        ExpectedError<void> BindComputeShader(const SharedPtr<Shader>& InShader, BindFunc&& InFunc)
        {
            m_BoundShader = InShader;
            m_BindlessResourcesToResolve.clear();
            ScopedCommandShader shader(m_BoundShader);
            return InFunc(*this, shader);
        }

        void StageBindlessResource(CommandShaderToken, std::string InBindingName, const GPUResourceManager::BufferUAVHandle InHandle);

        ExpectedError<void> Dispatch(const uint3 InDispatchConfig);

    private:

        struct StagedBindlessResource
        {
            std::string Name;
            GPUResourceManager::DescriptorOpaqueHandle Descriptor;
        };

        ExpectedError<void> ResolveAndStageBindlessResources();

        SharedPtr<CommandList> m_List = nullptr;
        SharedPtr<Shader> m_BoundShader = nullptr;
        UniquePtr<ScopedGPUResourceManager> m_ResourceManager = nullptr;

        std::vector<StagedBindlessResource> m_BindlessResourcesToResolve;
    };

    class CommandEngine
        : public Object
    {
    public:

        struct CreationParams
        {
            SharedPtr<GPUDevice> Device;
        };

        CommandEngine(ObjectToken, const CreationParams& InParams);

        template<CommandEngineFuncType InLambdaType>
        ExpectedError<void> Execute(const InLambdaType& InFunc)
        {
            auto allocator = [this]()
                {
                    if (m_Allocators.size() == 0 || !m_Queue->HasFencePointBeenReached(m_Allocators.front().FencePoint))
                    {
                        return m_Device->CreateCommandAllocator
                        (
                            D3D12_COMMAND_LIST_TYPE_DIRECT,
                            "Command Allocator"
                        );
                    }

                    return std::move(ThrowIfUnexpected(m_Allocators.pop_front()).Allocator);
                }();

            m_List->Reset(allocator);
            ScopedCommandContext context(CommandEngineToken{}, m_List
                , m_ResourceManager
            );
            return InFunc(context)
                .and_then(
                    [&]() -> ExpectedError<void>
                    {
                        m_Allocators.push_back(FencedAllocator{ std::move(allocator), m_Queue->Signal() });
                        m_Queue->ExecuteCommandList(*m_List);
                        return {};
                    }
                )
                .or_else(
                    [&](Error&& InError) -> ExpectedError<void>
                    {
                        m_Allocators.push_back(FencedAllocator{ std::move(allocator), m_Queue->Signal() });
                        return Unexpected{ InError };
                    }
                );
        }

        template<ExecuteLambdaType InLambdaType>
        ExpectedError<void> Execute(InLambdaType&& InFunc)
        {
            auto allocator = [this]()
                {
                    if (m_Allocators.size() == 0 || !m_Queue->HasFencePointBeenReached(m_Allocators.front().FencePoint))
                    {
                        return m_Device->CreateCommandAllocator
                        (
                            D3D12_COMMAND_LIST_TYPE_DIRECT,
                            "Command Allocator"
                        );
                    }

                    return std::move(ThrowIfUnexpected(m_Allocators.pop_front()).Allocator);
                }();

            m_List->Reset(*allocator);
            ScopedCommandContext context(CommandEngineToken{}, m_List
                , m_ResourceManager
            );

            return InFunc(context)
                .and_then(
                    [&]() -> ExpectedError<void>
                    {
                        m_Allocators.push_back(FencedAllocator{ std::move(allocator), m_Queue->Signal() });
                        m_Queue->ExecuteCommandList(*m_List);
                        return {};
                    }
                )
                .or_else(
                    [&](Error&& InError) -> ExpectedError<void>
                    {
                        m_Allocators.push_back(FencedAllocator{ std::move(allocator), m_Queue->Signal() });
                        return Unexpected{ InError };
                    }
                );
        }

        template<ExecuteLambdaType InLambdaType>
        ExpectedError<void> Execute(const std::string_view InName, InLambdaType&& InFunc)
        {
            PIXScopedEvent(m_Queue->GetRaw(), 0ull, "%s", InName.data());
            return Execute(std::forward<InLambdaType>(InFunc));
        }

        template<ExecuteReadbackType InFuncType>
        ExpectedError<void> ExecuteReadback(const std::string_view InName, const GPUResourceManager::ReadbackResultHandle InReadbackHandle, InFuncType&& InFunc)
        {
            PIXScopedEvent(m_Queue->GetRaw(), 0ull, "%s", InName.data());
            return m_ResourceManager->ExecuteReadback(InReadbackHandle, std::forward<InFuncType>(InFunc));
        }

        void Flush();

    private:

        struct FencedAllocator
        {
            SharedPtr<CommandAllocator> Allocator;
            Fence::FencePoint FencePoint;
        };

        SharedPtr<GPUDevice> m_Device;
        SharedPtr<CommandQueue> m_Queue;
        SharedPtr<CommandList> m_List;
        SharedPtr<GPUResourceManager> m_ResourceManager;
        RingBuffer<FencedAllocator> m_Allocators;
    };
}