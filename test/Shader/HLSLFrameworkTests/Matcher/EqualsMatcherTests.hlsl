#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/STF/Matcher.hlsli"


struct TestStruct
{
    int Value;
    bool operator==(TestStruct InOther)
    {
        return Value == InOther.Value;
    }
};

[numthreads(1,1,1)]
void EqualsMatcherTests()
{
    SCENARIO("Equals Matcher Tests")
    {
        SECTION("two equal ints are equal")
        {
            ASSERT(IsTrue, stf::Equals(4).Matches(4));
        }

        SECTION("two different ints are not equal")
        {
            ASSERT(IsFalse, stf::Equals(4).Matches(5));
        }

        SECTION("two equal float4s are equal")
        {
            ASSERT(IsTrue, stf::Equals(float4(1.0, 2.0, 3.0, 4.0)).Matches(float4(1.0, 2.0, 3.0, 4.0)));
        }

        SECTION("two different float4s are not equal")
        {
            ASSERT(IsFalse, stf::Equals(float4(1.0, 2.0, 3.0, 4.5)).Matches(float4(1.0, 2.0, 3.0, 4.0)));
        }

        SECTION("two equal objects with operator== overload are equal")
        {
            TestStruct left;
            TestStruct right;

            left.Value = 42;
            right.Value = 42;
            ASSERT(IsTrue, stf::Equals(right).Matches(left));
        }

        SECTION("two different objects with operator== overload are not equal")
        {
            TestStruct left;
            TestStruct right;

            left.Value = 42;
            right.Value = 24;
            ASSERT(IsFalse, stf::Equals(right).Matches(left));
        }
    }
}