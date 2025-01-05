#pragma once

#include "Platform.h"

#include "D3D12/BindlessFreeListAllocator.h"
#include "D3D12/DescriptorHeap.h"
#include "D3D12/CommandList.h"
#include "D3D12/GPUDevice.h"
#include "D3D12/GPUResource.h"

#include "Utility/Expected.h"
#include "Utility/Object.h"
#include "Utility/Pointer.h"

namespace stf
{

    struct ShaderTestUAV
    {
        SharedPtr<GPUResource> Resource;
        BindlessFreeListAllocator::BindlessIndex Handle;
    };

    class ShaderTestDescriptorManager 
        : public Object
    {
    public:

        struct CreationParams
        {
            SharedPtr<GPUDevice> Device;
            u32 InitialSize = 16u;
        };

        enum class EErrorType
        {
            Unknown,
            AllocatorFull,
            AttemptedShrink,
            DescriptorAlreadyFree
        };

        template<typename T>
        using Expected = Expected<T, EErrorType>;

        ShaderTestDescriptorManager(ObjectToken, CreationParams InParams);

        [[nodiscard]] Expected<ShaderTestUAV> CreateUAV(SharedPtr<GPUResource> InResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc);
        Expected<void> ReleaseUAV(const ShaderTestUAV& InUAV);

        [[nodiscard]] Expected<SharedPtr<DescriptorHeap>> Resize(const u32 InNewSize);

        u32 GetSize() const;
        u32 GetCapacity() const;

        void SetDescriptorHeap(CommandList& InCommandList);

    private:

        SharedPtr<GPUDevice> m_Device;
        SharedPtr<DescriptorHeap> m_GPUHeap;
        SharedPtr<DescriptorHeap> m_CPUHeap;
        BindlessFreeListAllocator m_Allocator;
        DescriptorRange m_CPUDescriptors;
        DescriptorRange m_GPUDescriptors;
    };
}