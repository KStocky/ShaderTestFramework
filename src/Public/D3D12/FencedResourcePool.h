
#pragma once

#include "Platform.h"

#include "Container/RingBuffer.h"
#include "D3D12/Fence.h"
#include "Utility/Object.h"
#include "Utility/Pointer.h"

#include <functional>

namespace stf
{
    template<typename T>
    class FencedResourcePool : Object
    {
    public:

    private:

        struct FencedResource
        {
            SharedPtr<T> Resource;
            Fence::FencePoint FencePoint;
        };

        RingBuffer<FencedResource> m_Pool;
        std::function<SharedPtr<T>()> m_CreateFunc;
    };
}