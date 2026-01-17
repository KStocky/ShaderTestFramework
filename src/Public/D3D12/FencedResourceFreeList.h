
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

    namespace Errors::FencedResourceFreeList
    {
        inline ErrorFragment StaleHandle(const u32 InExpectedVersion, const u32 InActualVersion)
        {
            return ErrorFragment::Make<"Handle version mismatch (Expected: {}, Actual Handle: {}). Likely a stale resource handle">(InExpectedVersion, InActualVersion);
        }

        inline ErrorFragment HandleOutOfRange(const u32 InIndex)
        {
            return ErrorFragment::Make<"Handle index ({}) out of range. Is this handle from another free list?">(InIndex);
        }
    }

    template<typename T>
    class FencedResourceFreeList
    {
    public:

        struct CreationParams
        {
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
            : m_Queue(InParams.Queue)
        {
        }

        [[nodiscard]] Handle Manage(T&& InResource)
        {
            TickDeferredReleases();

            return m_FreeList.pop_front()
                .and_then(
                    [&](const u32VersionedIndex InVersionedIndex) -> ExpectedError<Handle>
                    {
                        const u32 index = InVersionedIndex.GetIndex();
                        const u32 version = InVersionedIndex.GetVersion();
                        auto& resource = m_Resources[index];

                        ThrowIfFalse(version == resource.Version);
                        resource.Resource = std::move(InResource);

                        return Handle{ FencedResourceFreeListToken<T>{}, InVersionedIndex };
                    }
                )
                .or_else(
                    [&](const Error&) -> ExpectedError<Handle>
                    {
                        const u32VersionedIndex versionedIndex{ static_cast<u32>(m_Resources.size()) };

                        m_Resources.emplace_back(std::move(InResource), versionedIndex.GetVersion());

                        return Handle{ FencedResourceFreeListToken<T>{}, versionedIndex };
                    }
                ).value();
        }

        ExpectedError<void> Release(const Handle InHandle)
        {
            return InternalValidateHandle(InHandle)
                .and_then(
                    [this](const u32VersionedIndex InVersionedIndex) -> ExpectedError<void>
                    {
                        const auto nextVersion = InVersionedIndex.Next();
                        const auto index = nextVersion.GetIndex();
                        m_Resources[index].Version = nextVersion.GetVersion();
                        m_DeferredReleasedHandles.push_back(
                            FencedResource
                            {
                                .VersionedIndex = nextVersion,
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

        ExpectedError<T> Get(const Handle InHandle) const
        {
            return InternalValidateHandle(InHandle)
                .and_then(
                    [this](const u32VersionedIndex InVersionedIndex) -> ExpectedError<T>
                    {
                        const auto index = InVersionedIndex.GetIndex();

                        return m_Resources[index].Resource;
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
                return Unexpected{ Error{ Errors::FencedResourceFreeList::HandleOutOfRange(index) } };
            }

            if (m_Resources[index].Version != version)
            {
                return Unexpected{ Error{ Errors::FencedResourceFreeList::StaleHandle(m_Resources[index].Version, version) } };
            }

            return versionedIndex;
        }

        void TickDeferredReleases()
        {
            while (!m_DeferredReleasedHandles.empty() && m_Queue->HasFencePointBeenReached(m_DeferredReleasedHandles.front().FencePoint))
            {
                const auto& releasedResource = ThrowIfUnexpected(m_DeferredReleasedHandles.pop_front());
                const auto versionedIndex = releasedResource.VersionedIndex;
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
            T Resource;
            u32 Version{};
        };

        std::vector<VersionedResource> m_Resources;
        RingBuffer<FencedResource> m_DeferredReleasedHandles;
        RingBuffer<u32VersionedIndex> m_FreeList;

        SharedPtr<CommandQueue> m_Queue;
    };
}