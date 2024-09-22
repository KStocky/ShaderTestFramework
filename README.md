# ShaderTestFramework

![Tests](https://github.com/KStocky/ShaderTestFramework/actions/workflows/BuildAndRunTests.yml/badge.svg)
![Link Validation](https://github.com/KStocky/ShaderTestFramework/actions/workflows/LinkValidation.yml/badge.svg)
![Minimum MSVC Version](https://byob.yarr.is/KStocky/ShaderTestFramework/MinMSVCVersion)
![Minimum CMake Version](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/KStocky/fc80ff760df6627ccc295d486a54824c/raw/MinCMakeVersion.json)

**Shader code is code. We should test it**

## What is Shader Test Framework

An automation testing framework for testing shader code. Based on Catch2. It uses D3D12 and requires HLSL 202x

It is a framework that is intended to be used with another testing framework such as [Catch2](https://github.com/catchorg/Catch2/tree/devel) to add shader testing capabilities to your already existing test suite. It is currently very experimental, meaning that every merge to main will likely break all old code. The purpose of this project is to explore how we can write unit tests in HLSL.

## Getting Started

This should be enough to grab the repo and build everything provided you have at the minimum Visual Studio and CMake versions specified by the badges above.
```
$ git clone https://github.com/KStocky/ShaderTestFramework
$ cd ShaderTestFramework
$ cmake --workflow --preset VS2022Build
```

From there you can have a play with the [examples](./examples).

There is also a much more in depth [tutorial](docs/Tutorial.md). This describes the requirements in detail, then takes you through how to include the framework in your project, and then how to write tests for your shader code.

Shader Test Framework also provides [Ninja Build](https://ninja-build.org/) presets for greater compilation speed. Docs on how to use Ninja with STF can be found in the [Installation Guide](./docs/InstallationGuide.md)

## Example Shader Unit Test

```c++
[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void OptionalTests()
{
    SCENARIO("GIVEN An Optional that is reset")
    {
        Optional<int> opt;
        opt.Reset();

        SECTION("THEN IsValid returns false")
        {
            ASSERT(IsFalse, opt.IsValid);
        }

        SECTION("THEN GetOrDefault returns default value")
        {
            const int expectedValue = 42;
            ASSERT(AreEqual, expectedValue, opt.GetOrDefault(expectedValue));
        }

        SECTION("WHEN value is set")
        {
            const int expectedValue = 42;
            opt.Set(expectedValue);

            SECTION("THEN IsValid returns true")
            {
                ASSERT(IsTrue, opt.IsValid);
            }

            SECTION("THEN GetOrDefault returns set value")
            {
                const int defaultValue = 24;
                ASSERT(AreEqual, expectedValue, opt.GetOrDefault(defaultValue));
            }
        }
    }
}
```

## How to use

- [Tutorial](./docs/Tutorial.md)
- [Reference](./docs/ShaderTestFramework.md)
- [Example Repository](https://github.com/KStocky/ShaderTestFrameworkExamples)
