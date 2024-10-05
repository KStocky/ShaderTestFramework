#include <D3D12/Shader/ShaderEnums.h>
#include <Framework/ShaderTestFixture.h>

#include <string>

#include <catch2/catch_test_macros.hpp>

SCENARIO("PowTests")
{
    // We need to enable GPU capturing to allow us to take a GPU capture during a test
    // This is done in the FixtureDesc.
    stf::ShaderTestFixture fixture(
        stf::ShaderTestFixture::FixtureDesc
        {
            .GPUDeviceParams
            {
                .EnableGPUCapture = true
            }
        }
    );

    // RunTest takes a desc that describes the test setup
    // In this case we give the HLSL source code, entry function name and thread group count.
    // We also enable a gpu capture
    REQUIRE(fixture.RunTest(
        stf::ShaderTestFixture::RuntimeTestDesc
        {
            .CompilationEnv
            {
                // The HLSL source can either be:
                // 1. a std::filesystem::path that points to the HLSL file under test
                // 2. a std::string containing the HLSL code under test.
                .Source = std::string{
                    R"(
                        // Include the test framework
                        #include "/Test/STF/ShaderTestFramework.hlsli"

                        int MyPow(int num, int power)
                        {
                            int ret = num;
                            for (int i = 1; i < power; ++i)
                            {
                                ret *= num;
                            }
                            return ret;
                        }

                        [numthreads(1, 1, 1)]
                        void RunPowTests()
                        {
                            //ASSERT(AreEqual, 1, MyPow(3, 0));
                            ASSERT(AreEqual, 3, MyPow(3, 1));
                            ASSERT(AreEqual, 9, MyPow(3, 2));
                            ASSERT(AreEqual, 27, MyPow(3, 3));
                            ASSERT(AreEqual, 81, MyPow(3, 4));
                        }
                    )"
                }
            },
            .TestName = "RunPowTests",
            .ThreadGroupCount{1, 1, 1},
            .GPUCaptureMode = stf::ShaderTestFixture::EGPUCaptureMode::On
        })
    );
}