#include <D3D12/Shader/ShaderEnums.h>
#include <Framework/ShaderTestFixture.h>

#include <string>

#include <catch2/catch_test_macros.hpp>

SCENARIO("PowTests")
{
    ShaderTestFixture::Desc desc{};

    desc.HLSLVersion = EHLSLVersion::v2021;

    // The HLSL source can either be:
    // 1. a std::filesystem::path that points to the HLSL file under test
    // 2. a std::string containing the HLSL code under test.
    desc.Source = std::string{
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

            [RootSignature(SHADER_TEST_RS)]
            [numthreads(1, 1, 1)]
            void RunPowTests()
            {
                //STF::AreEqual(1, MyPow(3, 0));
                STF::AreEqual(3, MyPow(3, 1));
                STF::AreEqual(9, MyPow(3, 2));
                STF::AreEqual(27, MyPow(3, 3));
                STF::AreEqual(81, MyPow(3, 4));
            }
        )"
    };
    ShaderTestFixture fixture(std::move(desc));
    fixture.TakeCapture();
    // RunTest takes the entry function of the shader to run and also the dispatch config.
    // In this case we are launching a single threadgroup
    REQUIRE(fixture.RunTest("RunPowTests", 1, 1, 1));
}