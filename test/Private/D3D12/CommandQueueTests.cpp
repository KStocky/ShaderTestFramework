#include <Platform.h>
#include <D3D12/CommandQueue.h>
#include <D3D12/GPUDevice.h>
#include <Utility/EnumReflection.h>

#include <functional>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

class CommandQueueTestFixture
{

protected:

    void BeginTestCase(const stf::GPUDevice::EDeviceType InType) const
    {
        device = stf::Object::New<stf::GPUDevice> (
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

TEST_CASE_PERSISTENT_FIXTURE( CommandQueueTestFixture, "Scenario: CommandQueueTests")
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

                            THEN("Waiting queue is no longer waiting")
                            {
                                REQUIRE(directQueue->HasFencePointBeenReached(waitingFencePoint));
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