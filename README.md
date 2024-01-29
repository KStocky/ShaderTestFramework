# ShaderTestFramework

![Tests](https://github.com/KStocky/ShaderTestFramework/actions/workflows/BuildAndRunTests.yml/badge.svg)

**Shader code is code. We should test it**

## What is Shader Test Framework

An automation testing framework for testing shader code. Based on Catch2. It uses D3D12 and requires HLSL 2021

It is a framework that is intended to be used with another testing framework such as [Catch2](https://github.com/catchorg/Catch2/tree/devel) to add shader testing capabilities to your already existing test suite. It is currently very experimental, meaning that every merge to main will likely break all old code. The purpose of this project is to explore how we can write unit tests in HLSL.

## Getting Started

This should be enough to grab the repo and build everything provided you have a recent install of Visual Studio 2022 and CMake.
```
$ git clone https://github.com/KStocky/ShaderTestFramework
$ cd ShaderTestFramework
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

From there you can have a play with the examples. 

There is also a much more in depth [tutorial](docs/Tutorial.md). This describes the requirements in detail, then takes you through how to include the framework in your project, and then how to write tests for your shader code.

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
[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void OptionalTestsWithScenariosAndSections()
{
    SCENARIO(/*GIVEN An Optional that is reset*/)
    {
        Optional<int> opt;
        opt.Reset();

        SECTION(/*THEN IsValid returns false*/)
        {
            STF::IsFalse(opt.IsValid);
        }

        SECTION(/*THEN GetOrDefault returns default value*/)
        {
            const int expectedValue = 42;
            STF::AreEqual(expectedValue, opt.GetOrDefault(expectedValue));
        }

        SECTION(/*WHEN value is set*/)
        {
            const int expectedValue = 42;
            opt.Set(expectedValue);

            SECTION(/*THEN IsValid returns true*/)
            {
                STF::IsTrue(opt.IsValid);
            }

            SECTION(/*THEN GetOrDefault returns set value*/)
            {
                const int defaultValue = 24;
                STF::AreEqual( expectedValue, opt.GetOrDefault(defaultValue));
            }
        }
    }
}
```
