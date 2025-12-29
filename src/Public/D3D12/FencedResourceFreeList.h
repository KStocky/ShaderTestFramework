
#pragma once

#include "Platform.h"

#include "Container/RingBuffer.h"
#include "D3D12/CommandQueue.h"
#include "D3D12/Fence.h"
#include "Utility/Error.h"
#include "Utility/Exception.h"
#include "Utility/Expected.h"
#include "Utility/Pointer.h"
#include "Utility/VersionedIndex.h"

#include <functional>
#include <type_traits>
#include <vector>

namespace stf
{
    template<typename T>
    class FencedResourceFreeList;

    template<typename T>
    class FencedResourceFreeListToken
    {
        friend class FencedResourceFreeList<T>;
        FencedResourceFreeListToken() {}
    };

    template<typename T>
    class FencedResourceFreeList
    {
    public:

        struct CreationParams
        {
            std::function<SharedPtr<T>()> CreateFunc;
            SharedPtr<CommandQueue> Queue;
        };

        class Handle
        {
        public:

            Handle(FencedResourceFreeListToken<T>, u32VersionedIndex InIndex)
                : m_Index(InIndex)
            {
            }

            u32VersionedIndex GetIndex(FencedResourceFreeListToken<T>) const
            {
                return m_Index;
            }

            friend bool operator==(const Handle&, const Handle&) = default;
            friend bool operator!=(const Handle&, const Handle&) = default;

        private:

            u32VersionedIndex m_Index;
        };

        FencedResourceFreeList(const CreationParams& InParams)
            : m_CreateFunc(InParams.CreateFunc)
            , m_Queue(InParams.Queue)
        {
        }

        [[nodiscard]] Handle Acquire()
        {
            TickDeferredReleases();
            if (m_FreeList.empty())
            {
                const u32VersionedIndex resourceIndex{ static_cast<u32>(m_Resources.size()) };
                
                m_Resources.emplace_back(m_CreateFunc(), resourceIndex.GetVersion());
                m_DeferredResources.emplace_back(false);

                return Handle{ FencedResourceFreeListToken<T> {}, resourceIndex };
            }

            return Handle{ FencedResourceFreeListToken<T>{}, ThrowIfUnexpected(m_FreeList.pop_front()) };
        }

        ExpectedError<void> Release(const Handle InHandle)
        {
            return InternalValidateHandle(InHandle)
                .and_then(
                    [this](const u32VersionedIndex InVersionedIndex) -> ExpectedError<void>
                    {
                        const auto index = InVersionedIndex.GetIndex();

                        m_DeferredResources[index] = true;
                        m_DeferredReleasedHandles.push_back(
                            FencedResource
                            {
                                .VersionedIndex = InVersionedIndex.Next(),
                                .FencePoint = m_Queue->Signal()
                            });

                        return {};
                    }
                );
        }

        ExpectedError<void> ValidateHandle(const Handle InHandle) const
        {
            return InternalValidateHandle(InHandle).transform([](const auto&) {});
        }

        ExpectedError<std::reference_wrapper<T>> Get(const Handle InHandle) const
        {
            return InternalValidateHandle(InHandle)
                .and_then(
                    [this](const u32VersionedIndex InVersionedIndex) -> ExpectedError<std::reference_wrapper<T>>
                    {
                        const auto index = InVersionedIndex.GetIndex();

                        return *m_Resources[index].Resource.get();
                    }
                );
        }

    private:

        ExpectedError<u32VersionedIndex> InternalValidateHandle(const Handle InHandle) const
        {
            const auto versionedIndex = InHandle.GetIndex(FencedResourceFreeListToken<T> {});
            const u32 index = versionedIndex.GetIndex();
            const u32 version = versionedIndex.GetVersion();

            if (index >= static_cast<u32>(m_Resources.size()))
            {
                return Unexpected{ Error{ErrorFragment::Make<"Handle index ({}) out of range">(index) } };
            }

            if (m_Resources[index].Version != version)
            {
                return Unexpected{ Error{ErrorFragment::Make<"Handle version mismatch (Expected: {}, Actual Handle: {}). Likely a stale resource handle">(m_Resources[index].Version, version) } };
            }

            if (m_DeferredResources[index])
            {
                return Unexpected{ Error{ErrorFragment::Make<"Handle index ({}) has already been released">(index) } };
            }

            return versionedIndex;
        }

        void TickDeferredReleases()
        {
            while (!m_DeferredReleasedHandles.empty() && m_Queue->HasFencePointBeenReached(m_DeferredReleasedHandles.front().FencePoint))
            {
                const auto& releasedResource = ThrowIfUnexpected(m_DeferredReleasedHandles.pop_front());
                const auto versionedIndex = releasedResource.VersionedIndex;
                const u32 index = versionedIndex.GetIndex();
        
                m_Resources[index].Version = versionedIndex.GetVersion();
                m_DeferredResources[index] = false;
                m_FreeList.push_back(versionedIndex);
            }
        }

        struct FencedResource
        {
            u32VersionedIndex VersionedIndex{};
            Fence::FencePoint FencePoint;
        };

        struct VersionedResource
        {
            SharedPtr<T> Resource;
            u32 Version{};
        };

        std::vector<VersionedResource> m_Resources;
        std::vector<bool> m_DeferredResources;
        RingBuffer<FencedResource> m_DeferredReleasedHandles;
        RingBuffer<u32VersionedIndex> m_FreeList;


        std::function<SharedPtr<T>()> m_CreateFunc;
        SharedPtr<CommandQueue> m_Queue;
    };
}