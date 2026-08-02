#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>

namespace
{
    class TestAssertionInterface
        : public stf::AssertionsV1::AssertionsV1Interface
    {
    public:
        static constexpr stf::StringLiteral AssertionLibraryVirtualPath{
            "/Tests/AssertionInterface/AssertionLibrary.hlsli"
        };
    };
}

SCENARIO("AssertionInterfaceTests - Assertion library virtual path is forwarded to shaders")
{
    using namespace stf;

    BasicShaderTestFixture<TestAssertionInterface> fixture{
        BasicShaderTestFixtureBase::FixtureDesc{
            .Mappings{ GetTestVirtualDirectoryMapping() }
        }
    };

    REQUIRE(fixture.RunCompileTimeTest(
        ShaderCompileTestDesc{
            .CompilationEnv{
                .Source = std::filesystem::path{
                    "/Tests/AssertionInterface/AssertionLibraryVirtualPathTests.hlsl"
                }
            },
            .TestName = "AssertionLibraryVirtualPath"
        }
    ));
}
