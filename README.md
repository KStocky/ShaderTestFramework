# ShaderTestFramework

![Tests](https://github.com/KStocky/ShaderTestFramework/actions/workflows/BuildAndRunTests.yml/badge.svg)

## What is Shader Test Framework

An automation testing framework for testing shader code. Based on Catch2. It uses D3D12 and requires HLSL 2021

It is a framework that is intended to be used with another testing framework such as [Catch2](https://github.com/catchorg/Catch2/tree/devel) to add shader testing capabilities to your already existing test suite. It is currently very experimental, meaning that every merge to main will likely break all old code. The purpose of this project is to explore how unit tests can look like when written in HLSL.

## Example test

```c++

SCENARIO("MinimalShaderTestExample")
{
    ShaderTestFixture::Desc desc{};

    desc.HLSLVersion = EHLSLVersion::v2021;
    desc.Source = std::string{
        R"(
            // Include the test framework
            #include "/Test/STF/ShaderTestFramework.hlsli"

            [RootSignature(SHADER_TEST_RS)]
            [numthreads(1, 1, 1)]
            void MinimalTestEntryFunction()
            {
                STF::AreEqual(42, 42);
            }
        )"
    };
    ShaderTestFixture fixture(std::move(desc));

    REQUIRE(fixture.RunTest("MinimalTestEntryFunction", 1, 1, 1));
}

```

## Example Catch2 Style HLSL Test

```c++
#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void Catch2StyleHLSLTest(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    int num1 = 0;
    int num2 = 0;
    int num3 = 0;
    int num4 = 0;
    
    SCENARIO()
    {
        BEGIN_SECTION
            ++num1;
        END_SECTION

        BEGIN_SECTION

            ++num2;

            BEGIN_SECTION
                ++num3;
            END_SECTION
        
            BEGIN_SECTION
                ++num4;
            END_SECTION
        END_SECTION

    }
    
    STF::AreEqual(1, num1);
    STF::AreEqual(2, num2);
    STF::AreEqual(1, num3);
    STF::AreEqual(1, num4);
}

```

## How to use it

A tutorial to get started can be found [here](docs/Tutorial.md)