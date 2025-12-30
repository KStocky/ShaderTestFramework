#pragma once

#include "Container/RingBuffer.h"
#include "D3D12/CommandAllocator.h"
#include "D3D12/CommandQueue.h"
#include "D3D12/GPUDevice.h"
#include "D3D12/GPUResourceManager.h"

#include "Utility/FunctionTraits.h"
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
    concept CommandEngineFuncType = LambdaType<T> && requires()
    {
        requires T::ParamTypes::Size == 1;
        requires std::same_as<typename T::ParamTypes::template Type<0>, ScopedCommandContext&>;
    };

    template<typename T>
    concept ExecuteLambdaType =
        !CommandEngineFuncType<T> &&
        TFuncTraits<T>::ParamTypes::Size == 1 &&
        std::is_same_v<typename TFuncTraits<T>::ParamTypes::template Type<0>, ScopedCommandContext&>;


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
                m_ResourceManager->Release(cb);
            }

            for (const auto& cbv : m_CBVs)
            {
                m_ResourceManager->Release(cbv);
            }
        }
    
        [[nodiscard]] GPUResourceManager::ConstantBufferViewHandle CreateCBV(const std::span<const std::byte> InData)
        {
            const auto buffer = m_ResourceManager->Acquire(GPUResourceManager::ConstantBufferDesc{ .RequestedSize = static_cast<u32>(InData.size_bytes()) });
            const auto cbv = m_ResourceManager->CreateCBV(buffer);
    
            m_ResourceManager->UploadData(InData, buffer);
    
            m_ConstantBuffers.push_back(buffer);
            m_CBVs.push_back(cbv);
    
            return cbv;
        }
    
    private:
    
        SharedPtr<GPUResourceManager> m_ResourceManager;
        std::vector<GPUResourceManager::ConstantBufferHandle> m_ConstantBuffers;
        std::vector<GPUResourceManager::ConstantBufferViewHandle> m_CBVs;
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
        void Section(const std::string_view InName, InLambdaType&& InFunc)
        {
            PIXScopedEvent(m_List->GetRaw(), PIX_COLOR(0, 255, 0), "%s", InName.data());
            InFunc(*this);
        }

        CommandList* GetList() const
        {
            return m_List.get();
        }


        [[nodiscard]] GPUResourceManager::ConstantBufferViewHandle CreateCBV(const std::span<const std::byte> InData)
        {
            return m_ResourceManager->CreateCBV(InData);
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
        void Execute(const InLambdaType& InFunc)
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
            InFunc(context);

            m_Allocators.push_back(FencedAllocator{ std::move(allocator), m_Queue->Signal() });
            m_Queue->ExecuteCommandList(*m_List);
        }

        template<ExecuteLambdaType InLambdaType>
        void Execute(InLambdaType&& InFunc)
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
            InFunc(context);

            m_Allocators.push_back(FencedAllocator{ std::move(allocator), m_Queue->Signal() });
            m_Queue->ExecuteCommandList(*m_List);
        }

        template<ExecuteLambdaType InLambdaType>
        void Execute(const std::string_view InName, InLambdaType&& InFunc)
        {
            PIXScopedEvent(m_Queue->GetRaw(), 0ull, "%s", InName.data());
            Execute(std::forward<InLambdaType>(InFunc));
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