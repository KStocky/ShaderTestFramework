#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>
#include <catch2/catch_test_macros.hpp>

#include <ranges>

class StampTestsFixture
    : public ShaderTestFixtureBaseFixture
{
public:

    StampTestsFixture()
        : ShaderTestFixtureBaseFixture(
            stf::ShaderTestFixture::FixtureDesc
            {
                .Mappings{ GetTestVirtualDirectoryMapping() },
                .GPUDeviceParams
                {
                    .DebugLevel = stf::GPUDevice::EDebugLevel::DebugLayerWithValidation,
                    .DeviceType = stf::GPUDevice::EDeviceType::Software
                }
            }
        )
    {
    }
};

TEST_CASE_PERSISTENT_FIXTURE(StampTestsFixture, "HLSLFrameworkTests - Macros - STAMP")
{

    using namespace stf;
    const std::string first = GENERATE("0", "1");
    const std::string second = GENERATE("0", "1");
    const std::string third = GENERATE("0", "1");

    const auto testName = 
        [&]() -> std::string
        {
            const std::array sections
            {
                first == "1",
                second == "1",
                third == "1"
            };

            const auto activeSections = sections
                | std::views::enumerate
                | std::views::filter([](const auto InVal) { return get<1>(InVal); })
                | std::views::keys
                | std::views::transform([](const auto InIndex) {return InIndex + 1; })
                | std::ranges::to<std::vector>();

            const auto numActive = activeSections.size();
            if (numActive == 0)
            {
                return "No sections active";
            }
            else if (numActive == 1)
            {
                return std::format("Section {} is active", activeSections.back());
            }
            else
            {
                std::stringstream stream;
                stream << "Sections ";
                for (const auto section : activeSections | std::views::take(numActive - 1))
                {
                    stream << section << ", ";
                }
                
                stream << " and " << activeSections.back() << " are active";

                return stream.str();
            }
        }();

    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(
            ShaderTestFixture::RuntimeTestDesc
            {
                .CompilationEnv
                {
                    .Source = fs::path("/Tests/Macros/Stamp.hlsl"),
                    .Defines =
                    {
                        ShaderMacro{.Name = "ENABLE_1", .Definition = first},
                        ShaderMacro{.Name = "ENABLE_2", .Definition = second},
                        ShaderMacro{.Name = "ENABLE_3", .Definition = third}
                    }
                },
                .TestName = "StampTests",
                .ThreadGroupCount{1, 1, 1}
            }
        ));
    }
}
