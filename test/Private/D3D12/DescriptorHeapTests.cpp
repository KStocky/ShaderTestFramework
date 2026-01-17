

#include <D3D12/GPUDevice.h>
#include <Utility/EnumReflection.h>
#include <Utility/Pointer.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace DescriptorHeapTestPrivate
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

TEST_CASE_PERSISTENT_FIXTURE(DescriptorHeapTestPrivate::Fixture, "Descriptor Heap Tests")
{
    SECTION("Incompatible Type and Flags")
    {
        const auto [type, flags] = GENERATE(
            table<D3D12_DESCRIPTOR_HEAP_TYPE, D3D12_DESCRIPTOR_HEAP_FLAGS>(
                {
                    std::tuple{ D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE },
                    std::tuple{ D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE }
                }
            )
        );

        GIVEN("Type: " << stf::Enum::UnscopedName(type))
        {
            AND_GIVEN("Flag: " << stf::Enum::UnscopedName(flags))
            {
                WHEN("Created")
                {
                    THEN("Throws")
                    {
                        REQUIRE_THROWS(
                            device->CreateDescriptorHeap(
                                {
                                    .Type = type,
                                    .NumDescriptors = 1,
                                    .Flags = flags,
                                    .NodeMask = 0
                                }
                            )
                        );
                    }
                }
            }
        }
    }

    SECTION("Valid non-empty Descriptor Heap parameters")
    {
        const auto [type, flags] = GENERATE(
            table<D3D12_DESCRIPTOR_HEAP_TYPE, D3D12_DESCRIPTOR_HEAP_FLAGS>(
                {
                    std::tuple{ D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE },
                    std::tuple{ D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE },
                    std::tuple{ D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE },
                    std::tuple{ D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE },
                    std::tuple{ D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_NONE },
                    std::tuple{ D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE },
                }
            )
        );

        GIVEN("Type: " << stf::Enum::UnscopedName(type))
        {
            AND_GIVEN("Flag: " << stf::Enum::UnscopedName(flags))
            {
                WHEN("Created")
                {
                    static constexpr stf::u32 expectedNumDescriptors = 5;
                    auto heap = device->CreateDescriptorHeap(
                        {
                            .Type = type,
                            .NumDescriptors = expectedNumDescriptors,
                            .Flags = flags,
                            .NodeMask = 0
                        }
                    );

                    THEN("Heap is as expected")
                    {
                        REQUIRE(heap);
                        REQUIRE(expectedNumDescriptors == heap->GetNumDescriptors());
                        REQUIRE(type == heap->GetType());
                        REQUIRE(flags == heap->GetAccess());
                    }

                    AND_WHEN("Full Heap range queried")
                    {
                        const auto fullHeapRange = heap->GetHeapRange();

                        THEN("Range is as expected")
                        {
                            REQUIRE(fullHeapRange.IsValidRange());
                            REQUIRE(fullHeapRange.GetSize() == expectedNumDescriptors);
                        }
                    }

                    AND_WHEN("Valid descriptor handle is queried")
                    {
                        static constexpr auto index = expectedNumDescriptors / 2;
                        const auto handle = heap->CreateDescriptorHandle(index);

                        THEN("Handle is as expected")
                        {
                            REQUIRE(handle);
                            REQUIRE(handle->GetHeapIndex() == index);
                        }
                    }

                    AND_WHEN("Valid sub range is queried")
                    {
                        static constexpr stf::u32 start = 1;
                        static constexpr auto numDescriptors = expectedNumDescriptors - 1;
                        const auto subRange = heap->CreateDescriptorRange(start, numDescriptors);

                        THEN("Sub range is as expected")
                        {
                            REQUIRE(subRange);
                            REQUIRE(subRange->IsValidRange());
                            REQUIRE(subRange->First()->GetHeapIndex() == start);
                            REQUIRE(subRange->GetSize() == numDescriptors);
                        }
                    }

                    AND_WHEN("Out of bounds descriptor handle is queried")
                    {
                        static constexpr auto index = expectedNumDescriptors + 1;
                        const auto handle = heap->CreateDescriptorHandle(index);

                        THEN("Handle is invalid")
                        {
                            REQUIRE_FALSE(handle);
                            REQUIRE(handle.error() == stf::DescriptorHeap::EErrorType::AllocationError);
                        }
                    }

                    AND_WHEN("Out of bounds descriptor range is queried")
                    {
                        static constexpr auto start = expectedNumDescriptors - 1;
                        static constexpr auto num = 2;
                        const auto subRange = heap->CreateDescriptorRange(start, num);

                        THEN("Sub range is invalid")
                        {
                            REQUIRE_FALSE(subRange);
                            REQUIRE(subRange.error() == stf::DescriptorHeap::EErrorType::AllocationError);
                        }
                    }
                }
            }
        }
    }
}
