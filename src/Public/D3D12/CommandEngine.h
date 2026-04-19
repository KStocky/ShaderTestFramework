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
        requires (T InFunc, ScopedCommandContext& InContext)
        {
            { InFunc(InContext) } -> ExpectedErrorType;
        };

    template<typename T>
    concept VoidExecuteLambdaType = ExecuteLambdaType<T> && 
        requires (T InFunc, ScopedCommandContext& InContext)
        {
            { InFunc(InContext) } -> ExpectedErrorWithValueType<void>;
        };

    template<typename T>
    concept BindShaderLambdaType = !LambdaType<T> &&
        TFuncTraits<T>::ParamTypes::Size == 1 &&
        std::is_same_v<typename TFuncTraits<T>::ParamTypes::template Type<0>, ScopedCommandShader&>&&
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

        ScopedCommandShader(
            const SharedPtr<Shader>& InShader, 
            const SharedPtr<ScopedGPUResourceManager>& InResourceManager,
            const SharedPtr<CommandList>& InList,
            const uint3 InDipatchConfig);
        ScopedCommandShader(const ScopedCommandShader&) = delete;
        ScopedCommandShader(ScopedCommandShader&&) = delete;
        ScopedCommandShader& operator=(const ScopedCommandShader&) = delete;
        ScopedCommandShader& operator=(ScopedCommandShader&&) = delete;

        ExpectedError<void> StageBindingData(const ShaderBinding& InBinding);
        ExpectedError<void> StageBindlessResource(std::string InBindingName, const GPUResourceManager::BufferUAVHandle InHandle);

        uint3 GetThreadgroupCount() const;
        uint3 GetThreadCount() const;
        uint3 GetThreadGroupSize() const;

    private:

        SharedPtr<Shader> m_Shader;
        SharedPtr<ScopedGPUResourceManager> m_ResourceManager;
        SharedPtr<CommandList> m_List;
        uint3 m_DispatchConfig;
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
        auto Section(const std::string_view InName, InLambdaType&& InFunc)
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
        ExpectedError<void> Dispatch(const uint3 InDispatchConfig, const SharedPtr<Shader>& InShader, BindFunc&& InFunc)
        {
            PreBindShader(InShader);
            ScopedCommandShader shader(InShader, m_ResourceManager, m_List, InDispatchConfig);

            return InFunc(shader)
                .transform(
                    [&]()
                    {
                        SetShaderStateAndDispatch(InShader, InDispatchConfig);
                    }
                );
        }

    private:

        void PreBindShader(const SharedPtr<Shader>& InShader);
        void SetShaderStateAndDispatch(const SharedPtr<Shader>& InShader, const uint3 InDispatchConfig);

        SharedPtr<CommandList> m_List = nullptr;
        SharedPtr<Shader> m_BoundShader = nullptr;
        SharedPtr<ScopedGPUResourceManager> m_ResourceManager = nullptr;
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

        template<ExecuteLambdaType InLambdaType>
        auto Execute(InLambdaType&& InFunc)
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

            return ExecuteImpl(context, std::move(allocator), std::forward<InLambdaType>(InFunc));
        }

        template<ExecuteLambdaType InLambdaType>
        auto Execute(const std::string_view InName, InLambdaType&& InFunc)
        {
            PIXScopedEvent(m_Queue->GetRaw(), 0ull, "%s", InName.data());
            return Execute(std::forward<InLambdaType>(InFunc));
        }

        template<ExecuteReadbackType InFuncType>
        auto ExecuteReadback(const GPUResourceManager::ReadbackResultHandle InReadbackHandle, InFuncType&& InFunc)
        {
            return m_ResourceManager->ExecuteReadback(InReadbackHandle, std::forward<InFuncType>(InFunc));
        }

        void Flush();

    private:

        template<VoidExecuteLambdaType InLambdaType>
        ExpectedError<void> ExecuteImpl(ScopedCommandContext& InContext, SharedPtr<CommandAllocator>&& InCommandAllocator, InLambdaType&& InFunc)
        {
            return InFunc(InContext)
                .and_then(
                    [&]() -> ExpectedError<void>
                    {
                        m_Allocators.push_back(FencedAllocator{ std::move(InCommandAllocator), m_Queue->Signal() });
                        m_Queue->ExecuteCommandList(*m_List);
                        return {};
                    }
                )
                .or_else(
                    [&](Error&& InError) -> ExpectedError<void>
                    {
                        m_List->Close();
                        m_Allocators.push_back(FencedAllocator{ std::move(InCommandAllocator), m_Queue->Signal() });
                        return Unexpected{ InError };
                    }
                );
        }

        template<ExecuteLambdaType InLambdaType>
            requires (!VoidExecuteLambdaType<InLambdaType>)
        auto ExecuteImpl(ScopedCommandContext& InContext, SharedPtr<CommandAllocator>&& InCommandAllocator, InLambdaType&& InFunc)
        {
            return InFunc(InContext)
                .transform(
                    [&](auto&& InResult)
                    {
                        m_Allocators.push_back(FencedAllocator{ std::move(InCommandAllocator), m_Queue->Signal() });
                        m_Queue->ExecuteCommandList(*m_List);
                        return InResult;
                    }
                )
                .transform_error(
                    [&](Error&& InError)
                    {
                        m_List->Close();
                        m_Allocators.push_back(FencedAllocator{ std::move(InCommandAllocator), m_Queue->Signal() });
                        return InError;
                    }
                );
        }

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