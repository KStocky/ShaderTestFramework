
#pragma once

#include "D3D12/CommandQueue.h"
#include "D3D12/DescriptorManager.h"
#include "D3D12/FencedResourceFreeList.h"
#include "D3D12/GPUDevice.h"
#include "D3D12/GPUResource.h"
#include "Utility/Object.h"
#include "Utility/Pointer.h"
#include "Utility/VersionedIndex.h"

#include <optional>
#include <vector>

namespace stf
{

    class CommandList;

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
            std::string_view Name = "DefaultConstantBuffer";
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
            std::string_view Name = "DefaultConstantBuffer";
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

        GPUResourceManager(ObjectToken InToken, const CreationParams& InParams);

        [[nodiscard]] BufferHandle Acquire(const BufferDesc& InDesc);
        [[nodiscard]] ConstantBufferHandle Acquire(const ConstantBufferDesc& InDesc);

        [[nodiscard]] BufferUAVHandle CreateUAV(const BufferHandle InHandle, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc);
        [[nodiscard]] ConstantBufferViewHandle CreateCBV(const ConstantBufferHandle InHandle);
        
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
    };
}