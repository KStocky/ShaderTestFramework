#include <Platform.h>
#include <D3D12/CommandQueue.h>
#include <D3D12/GPUDevice.h>
#include <Utility/EnumReflection.h>

#include <functional>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("CommandQueueTests")
{
    using namespace stf;

    const auto deviceType = GENERATE
    (
        GPUDevice::EDeviceType::Hardware, 
        GPUDevice::EDeviceType::Software
    );

    GIVEN("DeviceType: " << Enum::UnscopedName(deviceType))
    {
        auto device = Object::New<GPUDevice>(
            GPUDevice::CreationParams
            {
                .DebugLevel = GPUDevice::EDebugLevel::DebugLayer,
                .DeviceType = deviceType,
                .EnableGPUCapture = false
            });

        AND_GIVEN("Two command queues created")
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

            THEN("queues are valid")
            {
                REQUIRE(directQueue);
                REQUIRE(copyQueue);
            }

            WHEN("Fence signalled with no work")
            {
                const auto fencePoint = directQueue->Signal();

                THEN("Fence point will have been reached immediately")
                {
                    REQUIRE(directQueue->HasFencePointBeenReached(fencePoint));
                }
            }

            WHEN("Next signalled fence point queried")
            {
                const auto futureFencePoint = copyQueue->NextSignal();

                THEN("Fence point will not have been reached")
                {
                    REQUIRE_FALSE(copyQueue->HasFencePointBeenReached(futureFencePoint));
                }

                AND_WHEN("Future fence point is waited on by another queue")
                {
                    directQueue->WaitOnFenceGPU(futureFencePoint);

                    AND_WHEN("The queue that is waiting is signalled")
                    {
                        const auto waitingFencePoint = directQueue->Signal();

                        THEN("Fence point is not reached")
                        {
                            REQUIRE_FALSE(directQueue->HasFencePointBeenReached(waitingFencePoint));
                        }

                        AND_WHEN("The future fence point is eventually signalled")
                        {
                            const auto actualFencePoint = copyQueue->Signal();

                            THEN("Future fence point has been reached")
                            {
                                REQUIRE(copyQueue->HasFencePointBeenReached(futureFencePoint));
                            }

                            THEN("Waiting queue still not signalled")
                            {
                                REQUIRE_FALSE(directQueue->HasFencePointBeenReached(waitingFencePoint));
                            }

                            AND_WHEN("wait for small amount of time")
                            {
                                const auto waitResult = directQueue->GetFence().WaitCPU(waitingFencePoint, Milliseconds<u32>{1u});

                                THEN("Waiting queue is no longer waiting")
                                {
                                    REQUIRE(waitResult.has_value());
                                    REQUIRE(waitResult.value() == Fence::ECPUWaitResult::WaitFenceReached);
                                    REQUIRE(directQueue->HasFencePointBeenReached(waitingFencePoint));
                                }
                            }
                            
                        }
                    }
                }
            }

            [[maybe_unused]] const auto directSignal = directQueue->Signal();
            [[maybe_unused]] const auto copySignal = copyQueue->Signal();
        }
    }
}