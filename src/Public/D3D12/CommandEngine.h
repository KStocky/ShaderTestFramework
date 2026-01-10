#pragma once

#include "Container/RingBuffer.h"
#include "D3D12/CommandAllocator.h"
#include "D3D12/CommandQueue.h"
#include "D3D12/GPUDevice.h"
#include "D3D12/GPUResourceManager.h"
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
        friend class ScopedCommandContext;
        CommandEngineToken() = default;
    };

    class CommandEngine;
    class ScopedCommandContext;

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


    class ScopedGPUResourceManager
    {
    public:
    
        ScopedGPUResourceManager(const SharedPtr<GPUResourceManager>& InResourceManager)
            : m_ResourceManager(InResourceManager)
        {
        }
    
        ~ScopedGPUResourceManager() noexcept 
        {
            for (const auto& cb : m_ConstantBuffers)
            {
                ThrowIfUnexpected(m_ResourceManager->Release(cb));
            }

            for (const auto& cbv : m_CBVs)
            {
                ThrowIfUnexpected(m_ResourceManager->Release(cbv));
            }

            for (const auto& buffer : m_Buffers)
            {
                ThrowIfUnexpected(m_ResourceManager->Release(buffer));
            }

            for (const auto& bufferUAV : m_BufferUAVs)
            {
                ThrowIfUnexpected(m_ResourceManager->Release(bufferUAV));
            }
        }
    
        [[nodiscard]] GPUResourceManager::ConstantBufferViewHandle CreateCBV(const std::span<const std::byte> InData)
        {
            const auto buffer = m_ResourceManager->Acquire(GPUResourceManager::ConstantBufferDesc{ .RequestedSize = static_cast<u32>(InData.size_bytes()) });
            const auto cbv = m_ResourceManager->CreateCBV(buffer);
    
            ThrowIfUnexpected(m_ResourceManager->UploadData(InData, buffer));
    
            m_ConstantBuffers.push_back(buffer);
            m_CBVs.push_back(cbv);
    
            return cbv;
        }

        [[nodiscard]] GPUResourceManager::BufferHandle CreateBuffer(const GPUResourceManager::BufferDesc& InBufferDesc)
        {
            return m_ResourceManager->Acquire(InBufferDesc);
        }

        [[nodiscard]] GPUResourceManager::BufferUAVHandle CreateUAV(const GPUResourceManager::BufferHandle& InBufferHandle, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc)
        {
            return m_ResourceManager->CreateUAV(InBufferHandle, InDesc);
        }

        void SetUAV(CommandList& InList, const GPUResourceManager::BufferUAVHandle InHandle)
        {
            ThrowIfUnexpected(m_ResourceManager->SetUAV(InList, InHandle));
        }

        void SetRootDescriptor(CommandList& InList, const u32 InRootParamIndex, const GPUResourceManager::ConstantBufferViewHandle InHandle)
        {
            ThrowIfUnexpected(m_ResourceManager->SetRootDescriptor(InList, InRootParamIndex, InHandle));
        }

        void SetDescriptorHeap(CommandList& InList)
        {
            m_ResourceManager->SetDescriptorHeap(InList);
        }
    
    private:
    
        SharedPtr<GPUResourceManager> m_ResourceManager;
        std::vector<GPUResourceManager::ConstantBufferHandle> m_ConstantBuffers;
        std::vector<GPUResourceManager::ConstantBufferViewHandle> m_CBVs;
        std::vector<GPUResourceManager::BufferHandle> m_Buffers;
        std::vector<GPUResourceManager::BufferUAVHandle> m_BufferUAVs;
    };

    class ScopedCommandContext
    {
    public:

        ScopedCommandContext(CommandEngineToken, 
            const SharedPtr<CommandList>& InList,
            const SharedPtr<GPUResourceManager>& InResourceManager
        )
            : m_List(InList)
            , m_ResourceManager(MakeUnique<ScopedGPUResourceManager>(InResourceManager))
        {
        }

        CommandList* operator->() const
        {
            return GetList();
        }

        CommandList& operator*() const
        {
            return *GetList();
        }

        template<ExecuteLambdaType InLambdaType>
        ExpectedError<void> Section(const std::string_view InName, InLambdaType&& InFunc)
        {
            PIXScopedEvent(m_List->GetRaw(), PIX_COLOR(0, 255, 0), "%s", InName.data());
            return InFunc(*this);
        }

        CommandList* GetList() const
        {
            return m_List.get();
        }

        [[nodiscard]] GPUResourceManager::BufferHandle CreateBuffer(const GPUResourceManager::BufferDesc& InDesc)
        {
            return m_ResourceManager->CreateBuffer(InDesc);
        }

        [[nodiscard]] GPUResourceManager::BufferUAVHandle CreateUAV(const GPUResourceManager::BufferHandle& InBufferHandle, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc)
        {
            return m_ResourceManager->CreateUAV(InBufferHandle, InDesc);
        }

        void SetUAV(const GPUResourceManager::BufferUAVHandle InHandle)
        {
            m_ResourceManager->SetUAV(*m_List, InHandle);
        }

        [[nodiscard]] GPUResourceManager::ConstantBufferViewHandle CreateCBV(const std::span<const std::byte> InData)
        {
            return m_ResourceManager->CreateCBV(InData);
        }

        void SetRootDescriptor(const u32 InRootParamIndex, const GPUResourceManager::ConstantBufferViewHandle InHandle)
        {
            m_ResourceManager->SetRootDescriptor(*m_List, InRootParamIndex, InHandle);
        }

        void Dispatch(const uint3 InDispatchConfig)
        {
            m_List->Dispatch(InDispatchConfig.x, InDispatchConfig.y, InDispatchConfig.z);
        }

    private:

        SharedPtr<CommandList> m_List = nullptr;
        UniquePtr<ScopedGPUResourceManager> m_ResourceManager = nullptr;
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