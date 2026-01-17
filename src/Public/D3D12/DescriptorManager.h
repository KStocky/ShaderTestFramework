#pragma once

#include "Platform.h"

#include "D3D12/BindlessFreeListAllocator.h"
#include "D3D12/DescriptorHeap.h"
#include "D3D12/CommandList.h"
#include "D3D12/GPUDevice.h"

#include "Utility/Error.h"
#include "Utility/Object.h"
#include "Utility/Pointer.h"

namespace stf
{
    namespace Errors
    {
        ErrorFragment DescriptorManagerIsFull();
        ErrorFragment UnknownDescriptorManagerError();

        ErrorFragment InvalidDescriptorManagerDescriptor(const u32 InIndex);
        ErrorFragment DescriptorManagerDescriptorNotAllocated(const u32 InIndex);
        ErrorFragment ShrinkAttemptedOnDescriptorManager(const u32 InCurrentSize, const u32 InRequestedSize);
    }

    class DescriptorManager
        : public Object
    {

        struct Token {};

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
            DescriptorAlreadyFree,
            DescriptorNotAllocated,
            DescriptorInvalid
        };

        class Descriptor
        {
        public:

            Descriptor(Token, const SharedPtr<DescriptorManager>& InOwner, BindlessFreeListAllocator::BindlessIndex InIndex);

            ExpectedError<DescriptorHandle> Resolve() const;

            DescriptorManager* GetOwner(Token) const;

            BindlessFreeListAllocator::BindlessIndex GetIndex(Token) const;

        private:

            SharedPtr<DescriptorManager> m_Owner = nullptr;
            BindlessFreeListAllocator::BindlessIndex m_Index;
        };

        DescriptorManager(ObjectToken, const CreationParams& InParams);


        ExpectedError<Descriptor> Acquire();
        ExpectedError<void> Release(const Descriptor& InDescriptor);

        ExpectedError<SharedPtr<DescriptorHeap>> Resize(const u32 InNewSize);

        u32 GetSize() const;
        u32 GetCapacity() const;

        void SetDescriptorHeap(CommandList& InCommandList);

        ExpectedError<DescriptorHandle> ResolveDescriptor(const BindlessFreeListAllocator::BindlessIndex InIndex) const;

    private:

        SharedPtr<GPUDevice> m_Device;
        SharedPtr<DescriptorHeap> m_GPUHeap;
        SharedPtr<DescriptorHeap> m_CPUHeap;
        BindlessFreeListAllocator m_Allocator;
    };
}