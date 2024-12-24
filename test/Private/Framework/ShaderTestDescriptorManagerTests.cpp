
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
            }
        }
    }
}