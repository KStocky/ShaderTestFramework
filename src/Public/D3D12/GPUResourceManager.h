
#pragma once

#include "Container/FreeList.h"
#include "D3D12/CommandQueue.h"
#include "D3D12/DescriptorManager.h"
#include "D3D12/FencedResourceFreeList.h"
#include "D3D12/GPUDevice.h"
#include "D3D12/GPUResource.h"
#include "Utility/Concepts.h"
#include "Utility/FunctionTraits.h"
#include "Utility/Object.h"
#include "Utility/Pointer.h"
#include "Utility/VersionedIndex.h"

#include <optional>
#include <string>

namespace stf
{
    template<typename T>
    concept ExecuteReadbackType =
        TFuncTraits<T>::ParamTypes::Size == 1 &&
        std::is_same_v<typename TFuncTraits<T>::ParamTypes::template Type<0>, const MappedResource&> &&
        std::is_same_v<typename TFuncTraits<T>::ReturnType, ExpectedError<void>>;

    class CommandList;

    namespace Errors::GPUResourceManager
    {
        ErrorFragment ReadbackHasNotBeenCompleted(const std::string_view InSourceName);
    }

    class GPUResourceManager
        : public Object
    {
        struct Private {};

    public:

        using ResourceManager = FencedResourceFreeList<SharedPtr<GPUResource>>;
        using DescriptorFreeList = FencedResourceFreeList<DescriptorManager::Descriptor>;
        using ResourceHandle = typename ResourceManager::Handle;
        using DescriptorOpaqueHandle = typename DescriptorFreeList::Handle;
        using DescriptorHeapReleaseManager = FencedResourceFreeList<SharedPtr<DescriptorHeap>>;

        struct CreationParams
        {
            SharedPtr<GPUDevice> Device;
            SharedPtr<CommandQueue> Queue;
        };

        struct ConstantBufferDesc
        {
            std::string Name = "DefaultConstantBuffer";
            u32 RequestedSize = 0u;
        };

        class ConstantBufferHandle
        {
        public:

            ConstantBufferHandle(Private, const ResourceHandle InHandle);

            ResourceHandle GetHandle() const;

        private:

            ResourceHandle m_Handle;
        };

        class ConstantBufferViewHandle
        {
        public:

            ConstantBufferViewHandle(Private, const ResourceHandle InBufferHandle, const DescriptorOpaqueHandle InCBVHandle);

            ResourceHandle GetBufferHandle() const;
            DescriptorOpaqueHandle GetCBVHandle() const;

        private:

            ResourceHandle m_BufferHandle;
            DescriptorOpaqueHandle m_CBVHandle;
        };

        struct BufferDesc
        {
            std::string Name = "DefaultBuffer";
            u32 RequestedSize = 0u;
            D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE;
        };

        class BufferHandle
        {
        public:

            BufferHandle(Private, const ResourceHandle InHandle);

            ResourceHandle GetHandle() const;

        private:

            ResourceHandle m_Handle;
        };

        class BufferUAVHandle
        {
        public:

            BufferUAVHandle(Private, const ResourceHandle InBufferHandle, const DescriptorOpaqueHandle InUAVHandle);

            ResourceHandle GetBufferHandle() const;
            DescriptorOpaqueHandle GetUAVHandle() const;

        private:

            ResourceHandle m_BufferHandle;
            DescriptorOpaqueHandle m_UAVHandle;
        };

        struct ReadbackBufferDesc
        {
            BufferHandle Source;
        };

        class ReadbackBufferHandle
        {
        public:

            ReadbackBufferHandle(Private, const ResourceHandle InReadbackHandle, const ResourceHandle InSourceHandle);

            ResourceHandle GetReadbackHandle() const;
            ResourceHandle GetSourceHandle() const;

        private:

            ResourceHandle m_ReadbackHandle;
            ResourceHandle m_SourceHandle;
        };

    private:

        struct InFlightReadback
        {
            ReadbackBufferHandle Handle;
            Fence::FencePoint FencePoint;
            std::string SourceBufferName;
        };

    public:

        using InFlightReadbackList = FreeList<InFlightReadback>;
        using InFlightReadbackHandle = InFlightReadbackList::Handle;

        class ReadbackResultHandle
        {
        public:

            ReadbackResultHandle(Private, const InFlightReadbackHandle InHandle);

            InFlightReadbackHandle GetReadbackHandle() const;

        private:
            InFlightReadbackHandle m_Handle;
        };

        GPUResourceManager(ObjectToken InToken, const CreationParams& InParams);

        [[nodiscard]] BufferHandle Acquire(const BufferDesc& InDesc);
        [[nodiscard]] ConstantBufferHandle Acquire(const ConstantBufferDesc& InDesc);
        ExpectedError<ReadbackBufferHandle> Acquire(const ReadbackBufferDesc& InDesc);

        [[nodiscard]] BufferUAVHandle CreateUAV(const BufferHandle InHandle, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc);
        [[nodiscard]] ConstantBufferViewHandle CreateCBV(const ConstantBufferHandle InHandle);

        [[nodiscard]] ExpectedError<u32> GetDescriptorIndex(const DescriptorOpaqueHandle InHandle) const;
        
        template<TriviallyCopyableType T>
        ExpectedError<void> UploadData(const T& InData, const ConstantBufferHandle InBufferHandle)
        {
            return UploadData(std::as_bytes(std::span<const T, 1>{ &InData }), InBufferHandle);
        }

        ExpectedError<void> UploadData(const std::span<const std::byte> InBytes, const ConstantBufferHandle InBufferHandle);

        ExpectedError<void> Release(const BufferHandle InHandle);
        ExpectedError<void> Release(const ConstantBufferHandle InHandle);
        ExpectedError<void> Release(const BufferUAVHandle InHandle);
        ExpectedError<void> Release(const ConstantBufferViewHandle InHandle);

        ExpectedError<ReadbackResultHandle> QueueReadback(CommandList& InCommandList, const ReadbackBufferHandle InHandle);

        template<ExecuteReadbackType FuncType>
        ExpectedError<void> ExecuteReadback(const ReadbackResultHandle InHandle, FuncType&& InFunc)
        {
            return m_Readbacks.Get(InHandle.GetReadbackHandle())
                .and_then(
                    [&](const InFlightReadback& InReadback)
                    {
                        if (!m_Queue->HasFencePointBeenReached(InReadback.FencePoint))
                        {
                            return Unexpected{ Errors::GPUResourceManager::ReadbackHasNotBeenCompleted(InReadback.SourceBufferName) };
                        }

                        return m_Resources.Get(InReadback.Handle.GetReadbackHandle())
                            .and_then(
                                [&](const SharedPtr<GPUResource>& InReadbackBuffer)
                                {
                                    return InFunc(InReadbackBuffer->Map());
                                })
                            .and_then(
                                [&]() -> ExpectedError<void>
                                {
                                    ThrowIfUnexpected(m_Resources.Release(InReadback.Handle.GetReadbackHandle()));
                                    ThrowIfUnexpected(m_Readbacks.Release(InHandle.GetReadbackHandle()));

                                    return {};
                                }
                            );
                    }
                );
        }

        ExpectedError<void> SetRootDescriptor(CommandList& InCommandList, const u32 InRootParamIndex, const ConstantBufferViewHandle InCBV);

        ExpectedError<void> SetUAV(CommandList& InCommandList, const BufferUAVHandle InHandle);

        void SetDescriptorHeap(CommandList& InCommandList);

    private:

        SharedPtr<GPUDevice> m_Device;
        SharedPtr<CommandQueue> m_Queue;
        SharedPtr<DescriptorManager> m_DescriptorManager;

        ResourceManager m_Resources;
        DescriptorFreeList m_Descriptors;
        DescriptorHeapReleaseManager m_HeapReleaseManager;

        InFlightReadbackList m_Readbacks;
    };
}