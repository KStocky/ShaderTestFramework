#include <catch2/catch_test_macros.hpp>

#include "ShaderTestFixture.h"

TEST_CASE("BasicTests")
{
    SECTION("This_Should_Succeed") 
    { 
        REQUIRE(0 == 0); 
    }

    SECTION("This_Should_Also_Succeed") 
    { 
        REQUIRE(0 != 42); 
    }
}

TEST_CASE("ShaderTestFixtureTests")
{
    SECTION("This_Should_Succeed")
    {
        ShaderTestFixture Test;
        REQUIRE(Test.IsValid());
        REQUIRE(Test.IsUsingAgilitySDK());
    }
}
