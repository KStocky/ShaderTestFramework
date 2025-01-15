
#pragma once

#include "Platform.h"

#include "Container/RingBuffer.h"
#include "D3D12/CommandQueue.h"
#include "D3D12/Fence.h"
#include "Utility/Exception.h"
#include "Utility/MoveOnly.h"
#include "Utility/Object.h"
#include "Utility/Pointer.h"

#include <functional>

namespace stf
{
    template<typename T>
    class FencedResourcePool;

    template<typename T>
    class FencedResourcePoolToken
    {
        friend class FencedResourcePool<T>;
        FencedResourcePoolToken() {}
    };

    template<typename T>
    class FencedResourcePool 
        : public Object
    {
    public:

        struct CreationParams
        {
            std::function<SharedPtr<T>()> CreateFunc;
            SharedPtr<CommandQueue> Queue;
        };

        class ScopedResource
            : MoveOnly
        {
        public:

            ScopedResource(FencedResourcePoolToken<T>, SharedPtr<T>&& InResource, SharedPtr<FencedResourcePool> InPool)
                : m_Resource(std::move(InResource))
                , m_Pool(std::move(InPool))
            {
            }

            ScopedResource(ScopedResource&& In) noexcept
                : m_Resource(std::exchange(In.m_Resource, nullptr))
                , m_Pool(std::exchange(In.m_Pool, nullptr))
            {
            }

            ScopedResource& operator=(ScopedResource&& In) noexcept
            {
                Release();
                m_Pool = std::exchange(In.m_Pool, nullptr);
                m_Resource = std::exchange(In.m_Resource, nullptr);
                return *this;
            }

            ~ScopedResource()
            {
                Release();
            }

            T* operator->() const
            {
                return m_Resource.get();
            }

            T& operator*() const
            {
                return *m_Resource;
            }

            operator T* () const
            {
                return m_Resource.get();
            }


        private:

            void Release()
            {
                if (m_Pool && m_Resource)
                {
                    m_Pool->Release(FencedResourcePoolToken<T>{}, std::move(m_Resource));
                }
            }

            SharedPtr<T> m_Resource;
            SharedPtr<FencedResourcePool> m_Pool;
        };

        FencedResourcePool(ObjectToken InToken, CreationParams InParams)
            : Object(InToken)
            , m_CreateFunc(std::move(InParams.CreateFunc))
            , m_Queue(std::move(InParams.Queue))
        {
        }

        ScopedResource Request()
        {
            if (m_Pool.size() == 0 || !m_Queue->HasFencePointBeenReached(m_Pool.front().FencePoint))
            {
                return ScopedResource{ FencedResourcePoolToken<T>{}, m_CreateFunc(), SharedFromThis() };
            }

            return ScopedResource{ FencedResourcePoolToken<T>{}, std::move(ThrowIfUnexpected(m_Pool.pop_front()).Resource), SharedFromThis() };
        }

        void Release(FencedResourcePoolToken<T>, SharedPtr<T>&& InResource)
        {
            m_Pool.push_back(FencedResource{ .Resource = std::move(InResource), .FencePoint = m_Queue->Signal() });
        }

    private:

        struct FencedResource
        {
            SharedPtr<T> Resource;
            Fence::FencePoint FencePoint;
        };

        RingBuffer<FencedResource> m_Pool;
        std::function<SharedPtr<T>()> m_CreateFunc;
        SharedPtr<CommandQueue> m_Queue;
    };
}