
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

        GPUResourceManager(ObjectToken InToken, const CreationParams& InParams);

        [[nodiscard]] ConstantBufferHandle Acquire(const ConstantBufferDesc InDesc);
        [[nodiscard]] ConstantBufferViewHandle CreateCBV(const ConstantBufferHandle InHandle);
        
        template<TriviallyCopyableType T>
        void UploadData(const T& InData, const ConstantBufferHandle InBufferHandle)
        {
            UploadData(std::as_bytes(std::span<const T, 1>{ &InData }), InBufferHandle);
        }

        void UploadData(const std::span<const std::byte> InBytes, const ConstantBufferHandle InBufferHandle);

        void Release(ConstantBufferHandle InHandle);
        void Release(ConstantBufferViewHandle InHandle);

        void SetCBV(CommandList& InCommandList, const u32 InRootParamIndex, const ConstantBufferViewHandle InCBV);

    private:

        SharedPtr<GPUDevice> m_Device;
        SharedPtr<CommandQueue> m_Queue;
        SharedPtr<DescriptorManager> m_DescriptorManager;

        ResourceManager m_Resources;
        DescriptorFreeList m_Descriptors;
        DescriptorHeapReleaseManager m_HeapReleaseManager;
    };
}