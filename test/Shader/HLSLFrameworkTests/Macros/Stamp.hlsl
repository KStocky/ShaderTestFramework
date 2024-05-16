
#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/TTL/macro.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void StampTests()
{
    SCENARIO("STAMP Tests")
    {
        SECTION("GIVEN: Stamper that does nothing, succeeds")
        {
            #define NOTHING(InN)
            #define DO_NOTHING(InStamper, InN) InStamper(0, NOTHING)

            TTL_STAMP(4, DO_NOTHING)

            TTL_STAMP(16, DO_NOTHING)

            TTL_STAMP(64, DO_NOTHING)

            TTL_STAMP(256, DO_NOTHING)

            #undef NOTHING
            #undef DO_NOTHING
        }
        
        SECTION("GIVEN: A stamper that adds 1 to a value")
        {
            #define ADD_TO_VALUE(InN) value += 1;
            #define STAMP_ADD(InStamper, InN) InStamper(1, ADD_TO_VALUE)
            int value = 0;
            
            SECTION("Stamp 4")
            {
                TTL_STAMP(4, STAMP_ADD)

                ASSERT(AreEqual, value, 4);
            }

            
            SECTION("Stamp 16")
            {
                TTL_STAMP(16, STAMP_ADD)

                ASSERT(AreEqual, value, 16);
            }

            
            SECTION("Stamp 64")
            {
                TTL_STAMP(64, STAMP_ADD)

                ASSERT(AreEqual, value, 64);
            }

            
            SECTION("Stamp 256")
            {
                TTL_STAMP(256, STAMP_ADD)

                ASSERT(AreEqual, value, 256);
            }

            #undef ADD_TO_VALUE
            #undef STAMP_ADD
        }

        SECTION("GIVEN: A stamper that adds 1 to a value passed as an Argument")
        {
            #define ADD_TO_VALUE(InN, InValue) InValue += 1;
            #define STAMP_ADD(InStamper, InN, InValue) InStamper(1, ADD_TO_VALUE, InValue)
            int value = 0;
            
            SECTION("Stamp 4")
            {
                TTL_STAMP(4, STAMP_ADD, value)

                ASSERT(AreEqual, value, 4);
            }

            
            SECTION("Stamp 16")
            {
                TTL_STAMP(16, STAMP_ADD, value)

                ASSERT(AreEqual, value, 16);
            }

            
            SECTION("Stamp 64")
            {
                TTL_STAMP(64, STAMP_ADD, value)

                ASSERT(AreEqual, value, 64);
            }

            
            SECTION("Stamp 256")
            {
                TTL_STAMP(256, STAMP_ADD, value)

                ASSERT(AreEqual, value, 256);
            }

            #undef ADD_TO_VALUE
            #undef STAMP_ADD
        }
    }
}