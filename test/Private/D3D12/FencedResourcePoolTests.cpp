
#include <Platform.h>
#include <D3D12/CommandQueue.h>
#include <D3D12/Descriptor.h>
#include <D3D12/FencedResourcePool.h>
#include <D3D12/GPUDevice.h>

#include "Utility/EnumReflection.h"
#include "Utility/Object.h"

#include <algorithm>
#include <functional>
#include <ranges>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

class FencedResourcePoolTestFixture
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

TEST_CASE_PERSISTENT_FIXTURE(FencedResourcePoolTestFixture, "Scenario: FencedResourcePoolTests")
{
    using namespace stf;

    const auto deviceType = GENERATE
    (
        GPUDevice::EDeviceType::Hardware,
        GPUDevice::EDeviceType::Software
    );

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
                    .Type = D3D12_COMMAND_LIST_TYPE_DIRECT
                }
            );

            auto copyQueue = device->CreateCommandQueue(
                D3D12_COMMAND_QUEUE_DESC
                {
                    .Type = D3D12_COMMAND_LIST_TYPE_COPY
                }
            );

            auto pool = Object::New<FencedResourcePool<i32>>(
                FencedResourcePool<i32>::CreationParams
                {
                    .CreateFunc = [id = 0]() mutable
                    {
                        return MakeShared<i32>(id++);
                    },
                    .Queue = directQueue
                });
            
            std::vector<decltype(pool->Request())> resources;

            THEN("pool and queues are valid")
            {
                REQUIRE(directQueue);
                REQUIRE(copyQueue);
                REQUIRE(pool);
            }

            WHEN("Resource requested")
            {
                resources.push_back(pool->Request());
            
                THEN("resource is as expected")
                {
                    REQUIRE(1 == resources.size());
                    REQUIRE(resources[0]);
                    REQUIRE(0 == *resources[0]);
                }

                AND_WHEN("resource is immediately released with no GPU work")
                {
                    resources.pop_back();

                    AND_WHEN("resource is requested again")
                    {
                        resources.push_back(pool->Request());
                        THEN("second resource is the same as the first")
                        {
                            REQUIRE(1 == resources.size());
                            REQUIRE(resources[0]);
                            REQUIRE(0 == *resources[0]);
                        }
                    }
                }

                AND_WHEN("Queue is executing work")
                {
                    const auto firstCopyFence = copyQueue->NextSignal();
                    directQueue->WaitOnFenceGPU(firstCopyFence);

                    THEN("Fence has not been reached")
                    {
                        REQUIRE_FALSE(copyQueue->HasFencePointBeenReached(firstCopyFence));
                    }

                    AND_WHEN("resource is released")
                    {
                        resources.pop_back();

                        AND_WHEN("resource is requested again")
                        {
                            resources.push_back(pool->Request());

                            THEN("second resource is different from the first")
                            {
                                REQUIRE(1 == resources.size());
                                REQUIRE(resources[0]);
                                REQUIRE(1 == *resources[0]);
                            }

                            AND_WHEN("yet another resource requested")
                            {
                                resources.push_back(pool->Request());

                                THEN("third resource is different from the second")
                                {
                                    REQUIRE(2 == resources.size());
                                    REQUIRE(resources[1]);
                                    REQUIRE(2 == *resources[1]);
                                }
                            }
                        }

                        AND_WHEN("GPU work has finished")
                        {
                            const auto finishedCopyFence = copyQueue->Signal();
                            const auto nextDirectFence = directQueue->Signal();
                            const auto waitResult = directQueue->WaitOnFenceCPU(nextDirectFence, Milliseconds<u32>{ 1u });

                            THEN("No longer waiting on fence")
                            {
                                REQUIRE(waitResult.has_value());
                                REQUIRE((waitResult.value() == Fence::ECPUWaitResult::FenceAlreadyReached || waitResult.value() == Fence::ECPUWaitResult::WaitFenceReached));
                                REQUIRE(directQueue->HasFencePointBeenReached(nextDirectFence));
                            }

                            AND_WHEN("resource is requested again")
                            {
                                resources.push_back(pool->Request());
                                THEN("second resource is the same as the first")
                                {
                                    REQUIRE(1 == resources.size());
                                    REQUIRE(resources[0]);
                                    REQUIRE(0 == *resources[0]);
                                }
                            }
                        }
                    }
                }
            }

            WHEN("Multiple resources are requested")
            {
                static constexpr i32 initialNumResources = 5;

                std::ranges::for_each(std::ranges::iota_view{ 0, initialNumResources },
                    [&](i32)
                    {
                        resources.push_back(pool->Request());
                    });

                THEN("resources are as expected")
                {
                    REQUIRE(initialNumResources == resources.size());
                    std::ranges::for_each(std::ranges::iota_view{ 0, initialNumResources },
                        [&](const i32 InIndex)
                        {
                            REQUIRE(InIndex == *resources[InIndex]);
                        });
                }

                AND_WHEN("Queue is executing work")
                {
                    const auto firstFutureFence = copyQueue->NextSignal();
                    directQueue->WaitOnFenceGPU(firstFutureFence);

                    THEN("Fence has not been reached yet")
                    {
                        REQUIRE_FALSE(copyQueue->HasFencePointBeenReached(firstFutureFence));
                    }

                    AND_WHEN("Middle resource is released")
                    {
                        resources.erase(resources.begin() + initialNumResources / 2);

                        AND_WHEN("queue finishes executing work")
                        {
                            const auto firstActualFence = copyQueue->Signal();
                            const auto nextDirectFence = directQueue->Signal();
                            const auto waitResult = directQueue->WaitOnFenceCPU(nextDirectFence, Milliseconds<u32>{ 1u });

                            THEN("queue is no longer executing work")
                            {
                                REQUIRE(waitResult.has_value());
                                REQUIRE((waitResult.value() == Fence::ECPUWaitResult::FenceAlreadyReached || waitResult.value() == Fence::ECPUWaitResult::WaitFenceReached));
                                REQUIRE(directQueue->HasFencePointBeenReached(nextDirectFence));
                            }

                            AND_WHEN("resource is requested")
                            {
                                resources.push_back(pool->Request());

                                THEN("resource is the same as released")
                                {
                                    REQUIRE(initialNumResources == resources.size());
                                    REQUIRE(initialNumResources / 2 == *resources.back());
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