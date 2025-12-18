
#include <D3D12/GPUDevice.h>
#include <D3D12/DescriptorManager.h>
#include <Utility/Pointer.h>

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <ranges>

namespace DescriptorManagerTestPrivate
{
    class Fixture
    {
    public:

        Fixture()
            : device(stf::Object::New<stf::GPUDevice>(
                stf::GPUDevice::CreationParams
                {
                }))
        {
        }

    protected:

        stf::SharedPtr<stf::GPUDevice> device;
    };
}

TEST_CASE_PERSISTENT_FIXTURE(DescriptorManagerTestPrivate::Fixture, "Descriptor Manager Tests")
{
    using namespace stf;
    GIVEN("An initial size of 1")
    {
        auto manager = Object::New<DescriptorManager>(
            DescriptorManager::CreationParams{
                .Device = device,
                .InitialSize = 1
            });


        REQUIRE(1 == manager->GetCapacity());
        REQUIRE(0 == manager->GetSize());

        WHEN("Allocation made")
        {
            auto firstAllocationResult = manager->Acquire();

            REQUIRE(firstAllocationResult.has_value());
            REQUIRE(1 == manager->GetCapacity());
            REQUIRE(1 == manager->GetSize());

            AND_WHEN("Allocation released")
            {
                auto firstReleaseResult = manager->Release(firstAllocationResult.value());

                THEN("Succeeds")
                {
                    REQUIRE(firstReleaseResult.has_value());
                    REQUIRE(1 == manager->GetCapacity());
                    REQUIRE(0 == manager->GetSize());
                }

                AND_WHEN("Allocation made")
                {
                    auto secondAllocationResult = manager->Acquire();

                    THEN("Succeeds")
                    {
                        REQUIRE(secondAllocationResult.has_value());
                        REQUIRE(1 == manager->GetCapacity());
                        REQUIRE(1 == manager->GetSize());
                    }
                }

                AND_WHEN("Same allocation released")
                {
                    auto secondReleaseResult = manager->Release(firstAllocationResult.value());

                    THEN("Fails")
                    {
                        REQUIRE_FALSE(secondReleaseResult.has_value());
                        REQUIRE(secondReleaseResult.error() == DescriptorManager::EErrorType::DescriptorAlreadyFree);
                        REQUIRE(1 == manager->GetCapacity());
                        REQUIRE(0 == manager->GetSize());
                    }
                }
            }

            AND_WHEN("Allocation made")
            {
                auto secondAllocationResult = manager->Acquire();

                THEN("Fails")
                {
                    REQUIRE_FALSE(secondAllocationResult.has_value());
                    REQUIRE(secondAllocationResult.error() == DescriptorManager::EErrorType::AllocatorFull);
                    REQUIRE(1 == manager->GetCapacity());
                    REQUIRE(1 == manager->GetSize());
                }
            }

            AND_WHEN("Manager is resized to 4")
            {
                auto firstResizeResult = manager->Resize(4);

                REQUIRE(firstResizeResult.has_value());
                REQUIRE(4 == manager->GetCapacity());
                REQUIRE(1 == manager->GetSize());

                AND_WHEN("Another allocation made")
                {
                    auto secondAllocationResult = manager->Acquire();

                    THEN("Succeeds")
                    {
                        REQUIRE(secondAllocationResult.has_value());
                        REQUIRE(4 == manager->GetCapacity());
                        REQUIRE(2 == manager->GetSize());
                    }
                }
            }
        }
    }

    GIVEN("A full descriptor manager")
    {
        constexpr u32 initialSize = 4;
        auto manager = Object::New<DescriptorManager>(
            DescriptorManager::CreationParams{
                .Device = device,
                .InitialSize = initialSize
            });
        using ResultType = decltype(manager->Acquire());

        const auto [descriptors, resolvedDescriptors] = 
            [&, this](const u32 InNum)
            {
                std::vector<DescriptorManager::Descriptor> descriptors;
                descriptors.reserve(InNum);

                std::vector<DescriptorHandle> resolvedDescriptors;
                resolvedDescriptors.reserve(InNum);

                for (u32 i = 0; i < InNum; ++i)
                {
                    auto maybeDescriptor = manager->Acquire();
                    REQUIRE(maybeDescriptor.has_value());
                    auto descriptor = maybeDescriptor.value();

                    auto maybeResolved = descriptor.Resolve();
                    REQUIRE(maybeResolved.has_value());

                    const auto resolvedDescriptor = maybeResolved.value();
                    resolvedDescriptors.push_back(resolvedDescriptor);
                    descriptors.push_back(descriptor);
                }

                return Tuple{ std::move(descriptors), std::move(resolvedDescriptors) };
            }(initialSize);

        REQUIRE(initialSize == manager->GetCapacity());
        REQUIRE(initialSize == manager->GetSize());

        THEN("All descriptors are unique")
        {
            for (i32 i = 0; i < (initialSize -1); ++i)
            {
                for (i32 j = i + 1; j < initialSize; ++j)
                {
                    REQUIRE(resolvedDescriptors[i] != resolvedDescriptors[j]);
                }
            }
        }

        WHEN("Allocation made")
        {
            auto allocationResult = manager->Acquire();

            THEN("Fails")
            {
                REQUIRE_FALSE(allocationResult.has_value());
                REQUIRE(allocationResult.error() == DescriptorManager::EErrorType::AllocatorFull);
                REQUIRE(4 == manager->GetCapacity());
                REQUIRE(4 == manager->GetSize());
            }
        }

        WHEN("Second descriptor released")
        {
            auto releaseResult = manager->Release(descriptors[1]);

            THEN("release succeeds")
            {
                REQUIRE(releaseResult.has_value());
                REQUIRE(initialSize == manager->GetCapacity());
                REQUIRE((initialSize - 1) == manager->GetSize());
            }

            AND_WHEN("Another allocation made")
            {
                auto secondAllocationResult = manager->Acquire();

                THEN("Allocation succeeds")
                {
                    REQUIRE(secondAllocationResult.has_value());
                    const auto maybeSecondResolved = secondAllocationResult.value().Resolve();
                    REQUIRE(maybeSecondResolved.has_value());

                    REQUIRE(initialSize == manager->GetCapacity());
                    REQUIRE(initialSize == manager->GetSize());
                    REQUIRE(maybeSecondResolved.value() == resolvedDescriptors[1]);
                }
            }
        }

        WHEN("Resized")
        {
            constexpr u32 newSize = initialSize + initialSize;
            auto resizeResult = manager->Resize(newSize);

            REQUIRE(newSize == manager->GetCapacity());
            REQUIRE(initialSize == manager->GetSize());

            AND_WHEN("Another allocation made")
            {
                auto secondAllocationResult = manager->Acquire();
                THEN("allocation succeeds")
                {
                    REQUIRE(secondAllocationResult.has_value());
                    const auto allocatedDescriptor = secondAllocationResult.value();
                    const auto maybeResolvedAllocatedDescriptor = allocatedDescriptor.Resolve();
                    REQUIRE(maybeResolvedAllocatedDescriptor.has_value());
                    const auto resolvedAllocatedDescriptor = maybeResolvedAllocatedDescriptor.value();
                    REQUIRE(newSize == manager->GetCapacity());
                    REQUIRE(initialSize + 1 == manager->GetSize());

                    for (const auto& resovledDescriptor : resolvedDescriptors)
                    {
                        REQUIRE(resovledDescriptor.GetHeapIndex() != resolvedAllocatedDescriptor.GetHeapIndex());
                    }
                }
            }

            AND_WHEN("descriptors are resolved again")
            {
                const auto newlyResolvedDescriptors = 
                    [&]()
                    {
                        std::vector<DescriptorHandle> ret;
                        ret.reserve(descriptors.size());

                        std::ranges::transform(descriptors, std::back_inserter(ret),
                            [](const DescriptorManager::Descriptor& InDescriptor)
                            {
                                const auto maybeResolved = InDescriptor.Resolve();
                                REQUIRE(maybeResolved.has_value());
                                return maybeResolved.value();
                            });

                        return ret;
                    }();

                THEN("newly resolved descriptors are not equal by have same heap index")
                {
                    for (const auto& [oldDescriptor, newDescriptor] : std::views::zip(resolvedDescriptors, newlyResolvedDescriptors))
                    {
                        REQUIRE(oldDescriptor != newDescriptor);
                        REQUIRE(oldDescriptor.GetHeapIndex() == newDescriptor.GetHeapIndex());
                    }
                }
            }
        }
    }
}