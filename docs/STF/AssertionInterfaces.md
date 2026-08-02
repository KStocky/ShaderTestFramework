[Reference](../ShaderTestFramework.md)

# Assertion Interfaces

An assertion interface defines the contract between the C++ test runner and the
assertion library used by a shader. It owns both sides of that contract:

1. The HLSL declarations and functions available to a shader.
2. The GPU resources required by those functions.
3. The bindings used to pass those resources to the shader.
4. The readbacks and processing used to produce the final test result.

The assertion interface is selected by the C++ fixture. This keeps the C++ and
HLSL sides of the assertion ABI together and prevents a test from accidentally
running a shader with an incompatible result layout.

## The Default AssertionsV1 Interface

The assertion interface currently provided by STF is
`stf::AssertionsV1::AssertionsV1Interface`. The
`stf::AssertionsV1::ShaderTestFixture` used throughout the tutorial, examples,
and other reference pages is a convenience alias:

```c++
namespace stf::AssertionsV1
{
    using ShaderTestFixture =
        BasicShaderTestFixture<AssertionsV1Interface>;
}
```

Therefore, code using `stf::AssertionsV1::ShaderTestFixture` uses the V1
assertion interface.

AssertionsV1 provides STF's current `ASSERT` implementation, assertion
functions, scenarios and sections, strings, byte readers and writers, GPU
buffer layouts, and result formatting.

There is no runtime registry or global interface switch. The fixture's C++
type selects the interface at compile time.

## Selecting the HLSL Assertion Library

Every assertion interface provides a compile-time virtual path to its HLSL
entry header:

```c++
static constexpr stf::StringLiteral AssertionLibraryVirtualPath{
    "/Test/STF/AssertionsV1/Framework.hlsli"
};
```

`BasicShaderTestFixture` passes this path to DXC when it compiles a shader. The
stable STF shader header forwards to the selected interface:

```hlsl
#include "/Test/STF/ShaderTestFramework.hlsli"
```

Shader tests should include this stable header rather than including an
interface implementation directly. Names declared by the selected assertion
library are then available to the shader.

The virtual path must resolve through the mappings in
`BasicShaderTestFixtureBase::FixtureDesc`. See
[Virtual Shader Directories](./VirtualShaderDirectories.md) for information
about copying shader assets and configuring mappings.

## The C++ Interface Contract

A custom type must satisfy `stf::assert::CAssertionInterfaceType`, declared in
[`AssertionInterface.h`](../../src/Public/Framework/AssertionInterface.h).
The type must provide the following members:

| Member | Purpose |
|---|---|
| `TestRunResultsType` | The processed result returned to the test framework. |
| `PerTestData` | Interface-specific configuration supplied in `RuntimeTestDesc`. |
| `GPUResourcesType` | Resources kept alive from setup through dispatch and readback. |
| `GPUReadbackResourcesType` | Handles kept alive between queued readbacks and result processing. |
| `AssertionLibraryVirtualPath` | A `static constexpr stf::StringLiteral` identifying the interface's HLSL entry header. |
| `GetAdditionalCompilerArgs()` | Returns interface-specific DXC arguments and defines. |
| `CreateGPUResources()` | Creates and initializes resources required by the HLSL implementation. |
| `BindShaderData()` | Binds interface resources and constants before dispatch. |
| `QueueReadbacks()` | Queues copies of result data from the GPU. |
| `ProcessReadbacks()` | Maps and interprets readback data to produce `TestRunResultsType`. |

The required function signatures are:

```c++
std::vector<std::wstring> GetAdditionalCompilerArgs() const;

stf::ExpectedError<GPUResourcesType> CreateGPUResources(
    stf::ScopedCommandContext& InContext,
    const PerTestData& InPerTestData);

stf::ExpectedError<void> BindShaderData(
    stf::ScopedCommandShader& InShader,
    const GPUResourcesType& InResources,
    const PerTestData& InPerTestData);

stf::ExpectedError<GPUReadbackResourcesType> QueueReadbacks(
    stf::ScopedCommandContext& InContext,
    const GPUResourcesType& InResources);

stf::ExpectedError<TestRunResultsType> ProcessReadbacks(
    stf::CommandEngine& InEngine,
    const GPUReadbackResourcesType& InReadbacks,
    const PerTestData& InPerTestData);
```

`TestRunResultsType` must be default initializable, equality comparable,
writeable to an `std::ostream`, and provide:

```c++
bool Succeeded() const;
```

`Succeeded()` determines the boolean result returned by the fixture to the
host test framework. Returning an `stf::Error` from any interface operation
instead reports a test-run error.

The interface lifecycle for a runtime test is:

1. Add `GetAdditionalCompilerArgs()` and the assertion-library path to the
   shader compilation environment.
2. Compile the shader.
3. Call `CreateGPUResources()`.
4. Call `BindShaderData()` and apply the test's other shader bindings.
5. Dispatch the shader.
6. Call `QueueReadbacks()`.
7. Flush the command engine and call `ProcessReadbacks()`.

## Extending AssertionsV1

If the V1 resource and result ABI is suitable, an interface can derive from
`AssertionsV1Interface` and select an HLSL header which extends the V1 shader
library:

```c++
#include <Framework/AssertionsV1/AssertionsV1Interface.h>
#include <Framework/BasicShaderTestFixture.h>

class MyAssertionInterface
    : public stf::AssertionsV1::AssertionsV1Interface
{
public:
    static constexpr stf::StringLiteral AssertionLibraryVirtualPath{
        "/MyAssertions/Framework.hlsli"
    };
};

using MyShaderTestFixture =
    stf::BasicShaderTestFixture<MyAssertionInterface>;

static_assert(stf::assert::CAssertionInterfaceType<MyAssertionInterface>);
```

The custom HLSL entry header can include V1 and add project-specific names:

```hlsl
#ifndef MY_ASSERTIONS_FRAMEWORK_HEADER
#define MY_ASSERTIONS_FRAMEWORK_HEADER

#include "/Test/STF/AssertionsV1/Framework.hlsli"
#include "/MyAssertions/ProjectAssertions.hlsli"

#endif
```

This approach reuses V1's buffers, bindings, readback processing, and results.
It is appropriate when adding shader-side helpers or assertions that can use
the existing V1 data format.

## Implementing an Interface From Scratch

An independent assertion interface declares the same contract without
inheriting from AssertionsV1:

```c++
#include <Framework/BasicShaderTestFixture.h>

class MyAssertionInterface
{
public:
    struct PerTestData
    {
        // Per-test settings used by this interface.
    };

    struct GPUResourcesType
    {
        // Handles for resources used during dispatch.
    };

    struct GPUReadbackResourcesType
    {
        // Handles for queued readbacks.
    };

    struct TestRunResultsType
    {
        bool Passed = false;

        bool Succeeded() const
        {
            return Passed;
        }

        friend bool operator==(
            const TestRunResultsType&,
            const TestRunResultsType&) = default;

        friend std::ostream& operator<<(
            std::ostream& InOut,
            const TestRunResultsType& InResults);
    };

    static constexpr stf::StringLiteral AssertionLibraryVirtualPath{
        "/MyAssertions/Framework.hlsli"
    };

    std::vector<std::wstring> GetAdditionalCompilerArgs() const;

    stf::ExpectedError<GPUResourcesType> CreateGPUResources(
        stf::ScopedCommandContext& InContext,
        const PerTestData& InPerTestData);

    stf::ExpectedError<void> BindShaderData(
        stf::ScopedCommandShader& InShader,
        const GPUResourcesType& InResources,
        const PerTestData& InPerTestData);

    stf::ExpectedError<GPUReadbackResourcesType> QueueReadbacks(
        stf::ScopedCommandContext& InContext,
        const GPUResourcesType& InResources);

    stf::ExpectedError<TestRunResultsType> ProcessReadbacks(
        stf::CommandEngine& InEngine,
        const GPUReadbackResourcesType& InReadbacks,
        const PerTestData& InPerTestData);
};

using MyShaderTestFixture =
    stf::BasicShaderTestFixture<MyAssertionInterface>;

static_assert(stf::assert::CAssertionInterfaceType<MyAssertionInterface>);
```

The omitted function bodies and resource handle members are specific to the
custom ABI. The HLSL entry header must declare resources and data layouts which
match the C++ implementation. In particular:

1. `CreateGPUResources()` must allocate everything used by the shader header.
2. Names bound by `BindShaderData()` must match the names declared in HLSL.
3. `GPUResourcesType` must keep resources alive until readbacks are queued.
4. `GPUReadbackResourcesType` must keep readback handles alive until processing.
5. `ProcessReadbacks()` must interpret exactly the format written by the shader.

Use `GetAdditionalCompilerArgs()` for values which must be shared with the
shader at compilation time, such as generated type identifiers or
interface-specific feature defines.

## Using a Custom Interface

Map the directory containing the custom HLSL entry header and instantiate the
generic fixture with the custom interface:

```c++
MyShaderTestFixture fixture{
    MyShaderTestFixture::FixtureDesc{
        .Mappings{
            stf::VirtualShaderDirectoryMapping{
                "/MyAssertions",
                std::filesystem::current_path() / "Shaders/MyAssertions"
            },
            stf::VirtualShaderDirectoryMapping{
                "/Shaders",
                std::filesystem::current_path() / "Shaders"
            }
        }
    }
};

REQUIRE(fixture.RunTest(
    MyShaderTestFixture::RuntimeTestDesc{
        .CompilationEnv{
            .Source = std::filesystem::path{"/Shaders/MyTests.hlsl"}
        },
        .TestName = "MyTests",
        .ThreadGroupCount{1, 1, 1},
        .PerTestData{/* interface-specific configuration */}
    }
));
```

If the interface has constructor state, pass an instance as the second fixture
constructor argument:

```c++
MyShaderTestFixture fixture{
    MyShaderTestFixture::FixtureDesc{/* mappings and device settings */},
    MyAssertionInterface{/* interface state */}
};
```

The shader remains independent of the concrete C++ fixture type:

```hlsl
#include "/Test/STF/ShaderTestFramework.hlsli"

[numthreads(1, 1, 1)]
void MyTests()
{
    // Use names provided by /MyAssertions/Framework.hlsli.
}
```

---

[Top](#assertion-interfaces)
