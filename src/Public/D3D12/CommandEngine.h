#pragma once

#include "Container/RingBuffer.h"
#include "D3D12/CommandAllocator.h"
#include "D3D12/CommandQueue.h"
#include "D3D12/GPUDevice.h"

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
        CommandEngineToken() = default;
    };

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

    class ScopedCommandContext
    {
    public:

        ScopedCommandContext(CommandEngineToken, CommandList* InList)
            : m_List(InList)
        {}

        CommandList* operator->() const
        {
            return m_List;
        }

        CommandList& operator*() const
        {
            return *m_List;
        }

        template<ExecuteLambdaType InLambdaType>
        void Section(const std::string_view InName, InLambdaType&& InFunc)
        {
            PIXScopedEvent(m_List->GetRaw(), PIX_COLOR(0, 255, 0), "%s", InName.data());
            InFunc(*this);
        }

    private:
        CommandList* m_List = nullptr;
    };

    class CommandEngine : Object
    {
    public:

        struct CreationParams
        {
            SharedPtr<CommandList> List;
            SharedPtr<CommandQueue> Queue;
            SharedPtr<GPUDevice> Device;
        };

        CommandEngine() = default;
        CommandEngine(CreationParams InParams);

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
            ScopedCommandContext context(CommandEngineToken{}, m_List.get());
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
            ScopedCommandContext context(CommandEngineToken{}, m_List.get());
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
        RingBuffer<FencedAllocator> m_Allocators;
    };
}