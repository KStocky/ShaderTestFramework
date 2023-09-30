#include <catch2/catch_test_macros.hpp>


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
