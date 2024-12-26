
#include <D3D12/GPUDevice.h>
#include <Framework/ShaderTestDescriptorManager.h>
#include <Utility/EnumReflection.h>
#include <Utility/Pointer.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace DescriptorManagerTestPrivate
{
    class Fixture
    {
    public:

        Fixture()
            : device(stf::MakeShared<stf::GPUDevice>(
                stf::GPUDevice::CreationParams
                {
                }))
            , resource(device->CreateCommittedResource(
                CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                CD3DX12_RESOURCE_DESC1::Buffer(1024, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
                D3D12_BARRIER_LAYOUT_UNDEFINED))
            , uavDesc(
                D3D12_UNORDERED_ACCESS_VIEW_DESC
                {
                    .Format = DXGI_FORMAT_R32_TYPELESS,
                    .ViewDimension = D3D12_UAV_DIMENSION_BUFFER,
                    .Buffer
                    {
                        .FirstElement = 0,
                        .NumElements = 15,
                        .StructureByteStride = 0,
                        .CounterOffsetInBytes = 0,
                        .Flags = D3D12_BUFFER_UAV_FLAG_RAW
                    }
                })
        {
        }

    protected:

        stf::SharedPtr<stf::GPUDevice> device;
        stf::SharedPtr<stf::GPUResource> resource;
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    };
}

TEST_CASE_PERSISTENT_FIXTURE(DescriptorManagerTestPrivate::Fixture, "Descriptor Manager Tests")
{
    using namespace stf;
    GIVEN("An initial size of 1")
    {
        auto manager = MakeShared<ShaderTestDescriptorManager>(
            ShaderTestDescriptorManager::CreationParams{
                .Device = device,
                .InitialSize = 1
            });

        THEN("State is as expected")
        {
            REQUIRE(1 == manager->GetCapacity());
            REQUIRE(0 == manager->GetSize());
        }

        WHEN("Allocation made")
        {
            auto firstAllocationResult = manager->CreateUAV(resource, uavDesc);

            THEN("Succeeds")
            {
                REQUIRE(firstAllocationResult.has_value());
                REQUIRE(1 == manager->GetCapacity());
                REQUIRE(1 == manager->GetSize());
            }

            AND_WHEN("Allocation released")
            {
                auto firstReleaseResult = manager->ReleaseUAV(firstAllocationResult.value());

                THEN("Succeeds")
                {
                    REQUIRE(firstReleaseResult.has_value());
                    REQUIRE(1 == manager->GetCapacity());
                    REQUIRE(0 == manager->GetSize());
                }

                AND_WHEN("Allocation made")
                {
                    auto secondAllocationResult = manager->CreateUAV(resource, uavDesc);

                    THEN("Succeeds")
                    {
                        REQUIRE(secondAllocationResult.has_value());
                        REQUIRE(1 == manager->GetCapacity());
                        REQUIRE(1 == manager->GetSize());
                    }
                }

                AND_WHEN("Same allocation released")
                {
                    auto secondReleaseResult = manager->ReleaseUAV(firstAllocationResult.value());

                    THEN("Fails")
                    {
                        REQUIRE_FALSE(secondReleaseResult.has_value());
                        REQUIRE(secondReleaseResult.error() == ShaderTestDescriptorManager::EErrorType::DescriptorAlreadyFree);
                        REQUIRE(1 == manager->GetCapacity());
                        REQUIRE(0 == manager->GetSize());
                    }
                }
            }

            AND_WHEN("Allocation made")
            {
                auto secondAllocationResult = manager->CreateUAV(resource, uavDesc);

                THEN("Fails")
                {
                    REQUIRE_FALSE(secondAllocationResult.has_value());
                    REQUIRE(secondAllocationResult.error() == ShaderTestDescriptorManager::EErrorType::AllocatorFull);
                    REQUIRE(1 == manager->GetCapacity());
                    REQUIRE(1 == manager->GetSize());
                }
            }

            AND_WHEN("Manager is resized to 4")
            {
                auto firstResizeResult = manager->Resize(4);

                THEN("Resize succeeds")
                {
                    REQUIRE(firstResizeResult.has_value());
                    REQUIRE(4 == manager->GetCapacity());
                    REQUIRE(1 == manager->GetSize());
                }

                AND_WHEN("Another allocation made")
                {
                    auto secondAllocationResult = manager->CreateUAV(resource, uavDesc);

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
        auto manager = MakeShared<ShaderTestDescriptorManager>(
            ShaderTestDescriptorManager::CreationParams{
                .Device = device,
                .InitialSize = 4
            });
        using ResultType = decltype(manager->CreateUAV(resource, uavDesc));

        std::vector<ResultType> uavs;

        for (i32 i = 0; i < 4; ++i)
        {
            uavs.push_back(manager->CreateUAV(resource, uavDesc));
        }

        THEN("State is as expected")
        {
            for (const auto& uav : uavs)
            {
                REQUIRE(uav.has_value());
            }

            REQUIRE(4 == manager->GetCapacity());
            REQUIRE(4 == manager->GetSize());
        }

        THEN("All uavs are unique")
        {
            for (i32 i = 0; i < 3; ++i)
            {
                for (i32 j = i + 1; j < 4; ++j)
                {
                    REQUIRE(uavs[i].value().Handle.GetIndex() != uavs[j].value().Handle.GetIndex());
                }
            }
        }

        WHEN("Allocation made")
        {
            auto allocationResult = manager->CreateUAV(resource, uavDesc);

            THEN("Fails")
            {
                REQUIRE_FALSE(allocationResult.has_value());
                REQUIRE(allocationResult.error() == ShaderTestDescriptorManager::EErrorType::AllocatorFull);
                REQUIRE(4 == manager->GetCapacity());
                REQUIRE(4 == manager->GetSize());
            }
        }

        WHEN("Second uav released")
        {
            auto releaseResult = manager->ReleaseUAV(uavs[1].value());

            THEN("release succeeds")
            {
                REQUIRE(releaseResult.has_value());
                REQUIRE(4 == manager->GetCapacity());
                REQUIRE(3 == manager->GetSize());
            }

            AND_WHEN("Another allocation made")
            {
                auto secondAllocationResult = manager->CreateUAV(resource, uavDesc);

                THEN("Allocation succeeds")
                {
                    REQUIRE(secondAllocationResult.has_value());
                    REQUIRE(4 == manager->GetCapacity());
                    REQUIRE(4 == manager->GetSize());
                    REQUIRE(secondAllocationResult.value().Handle.GetIndex() == uavs[1].value().Handle.GetIndex());
                }
            }
        }

        WHEN("Resized")
        {
            auto resizeResult = manager->Resize(8);

            THEN("State is as expected")
            {
                REQUIRE(8 == manager->GetCapacity());
                REQUIRE(4 == manager->GetSize());
            }

            AND_WHEN("Another allocation made")
            {
                auto secondAllocationResult = manager->CreateUAV(resource, uavDesc);
                THEN("allocation succeeds")
                {
                    REQUIRE(secondAllocationResult.has_value());
                    REQUIRE(8 == manager->GetCapacity());
                    REQUIRE(5 == manager->GetSize());

                    for (const auto& uav : uavs)
                    {
                        REQUIRE(uav.value().Handle.GetIndex() != secondAllocationResult.value().Handle.GetIndex());
                    }
                }
            }
        }
    }
}