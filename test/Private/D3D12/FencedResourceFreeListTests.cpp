
#include <Platform.h>
#include <D3D12/CommandQueue.h>
#include <D3D12/Descriptor.h>
#include <D3D12/FencedResourceFreeList.h>
#include <D3D12/GPUDevice.h>

#include "Utility/EnumReflection.h"
#include "Utility/Object.h"

#include <algorithm>
#include <functional>
#include <ranges>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

class FencedResourceFreeListTestFixture
{
protected:

    void BeginTestCase(const stf::GPUDevice::EDeviceType InType) const
    {
        device = stf::Object::New<stf::GPUDevice>(
            stf::GPUDevice::CreationParams
            {
                .DeviceType = InType
            });
    }

    void EndTestCase() const
    {
        device = nullptr;
    }

    mutable stf::SharedPtr<stf::GPUDevice> device;
};

TEST_CASE_PERSISTENT_FIXTURE(FencedResourceFreeListTestFixture, "Scenario: FencedResourceFreeListTests")
{
    using namespace stf;

    using FreeListType = FencedResourceFreeList<i32>;

    const auto deviceType = GENERATE
    (
        GPUDevice::EDeviceType::Hardware,
        GPUDevice::EDeviceType::Software
    );

    auto getResource =
        [](FreeListType& InFreeList, const FreeListType::Handle InHandle)
        {
            const auto ret = InFreeList.Get(InHandle);
            REQUIRE(ret);
            return ret.value().get();
        };

    GIVEN("DeviceType: " << Enum::UnscopedName(deviceType))
    {
        SECTION("Setup")
        {
            REQUIRE_FALSE(device);
            BeginTestCase(deviceType);
            REQUIRE(device);
        }

        AND_GIVEN("An empty FencedResourcePool and two command queues created")
        {
            auto directQueue = device->CreateCommandQueue(
                D3D12_COMMAND_QUEUE_DESC
                {
                    .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
                    .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
                    .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
                    .NodeMask = 0
                }
            );

            REQUIRE(directQueue);

            auto copyQueue = device->CreateCommandQueue(
                D3D12_COMMAND_QUEUE_DESC
                {
                    .Type = D3D12_COMMAND_LIST_TYPE_COPY,
                    .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
                    .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
                    .NodeMask = 0
                }
            );

            REQUIRE(copyQueue);

            FreeListType freeList{
                FreeListType::CreationParams
                {
                    .CreateFunc = [id = 0]() mutable
                    {
                        return MakeShared<i32>(id++);
                    },
                    .Queue = directQueue
                } };

            WHEN("Resource requested")
            {
                const auto firstHandle = freeList.Acquire();

                REQUIRE(freeList.ValidateHandle(firstHandle));
                const auto firstResource = getResource(freeList, firstHandle);

                AND_WHEN("resource is immediately released with no GPU work")
                {
                    const auto releaseResult = freeList.Release(firstHandle);
                    REQUIRE(releaseResult);

                    THEN("handle is no longer valid")
                    {
                        REQUIRE_FALSE(freeList.ValidateHandle(firstHandle));
                        REQUIRE_FALSE(freeList.Get(firstHandle));
                    }

                    AND_WHEN("resource is acquired again")
                    {
                        const auto secondHandle = freeList.Acquire();
                        REQUIRE(freeList.ValidateHandle(secondHandle));
                        const auto secondResource = getResource(freeList, secondHandle);

                        THEN("second handle is to the same resource as the first")
                        {
                            REQUIRE(firstResource == secondResource);
                        }
                    }
                }

                AND_WHEN("Queue is executing work")
                {
                    const auto firstCopyFence = copyQueue->NextSignal();
                    directQueue->WaitOnFenceGPU(firstCopyFence);

                    REQUIRE_FALSE(copyQueue->HasFencePointBeenReached(firstCopyFence));

                    AND_WHEN("resource is released")
                    {
                        const auto releaseResult = freeList.Release(firstHandle);
                        REQUIRE(releaseResult);

                        AND_WHEN("resource is requested again")
                        {
                            const auto secondHandle = freeList.Acquire();
                            REQUIRE(freeList.ValidateHandle(secondHandle));
                            const auto secondResource = getResource(freeList, secondHandle);

                            THEN("second handle is to a different resource from the first")
                            {
                                REQUIRE(firstResource != secondResource);
                            }

                            AND_WHEN("yet another resource requested")
                            {
                                const auto thirdHandle = freeList.Acquire();
                                REQUIRE(freeList.ValidateHandle(thirdHandle));
                                const auto thirdResource = getResource(freeList, thirdHandle);

                                THEN("third resource is different from the second")
                                {
                                    REQUIRE(thirdResource != secondResource);
                                }
                            }
                        }

                        AND_WHEN("GPU work has finished")
                        {
                            [[maybe_unused]] const auto finishedCopyFence = copyQueue->Signal();
                            const auto nextDirectFence = directQueue->Signal();
                            const auto waitResult = directQueue->WaitOnFenceCPU(nextDirectFence, Milliseconds<u32>{ 1u });

                            REQUIRE(waitResult.has_value());
                            REQUIRE((waitResult.value() == Fence::ECPUWaitResult::FenceAlreadyReached || waitResult.value() == Fence::ECPUWaitResult::WaitFenceReached));
                            REQUIRE(directQueue->HasFencePointBeenReached(nextDirectFence));

                            AND_WHEN("resource is requested again")
                            {
                                const auto secondHandle = freeList.Acquire();
                                REQUIRE(freeList.ValidateHandle(secondHandle));
                                const auto secondResource = getResource(freeList, secondHandle);

                                THEN("second handle is to the same resource as the first")
                                {
                                    REQUIRE(firstResource == secondResource);
                                }
                            }
                        }
                    }
                }
            }

            [[maybe_unused]] const auto directSignal = directQueue->Signal();
            [[maybe_unused]] const auto copySignal = copyQueue->Signal();
        }

        SECTION("Teardown")
        {
            EndTestCase();
        }
    }
}