#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - TypeTraits - TypeID")
{
    static constexpr std::string_view testName = "NoDuplicateTypeIDs";
    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/TypeTraits/TypeIDTests.hlsl")));
    REQUIRE(fixture.RunCompileTimeTest(testName));
}