#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/STF/Matcher.hlsli"


struct TestStruct
{
    int Value;
    bool operator<(TestStruct InOther)
    {
        return Value < InOther.Value;
    }
};

[numthreads(1,1,1)]
void LessThanMatcherTests()
{
    SCENARIO("Less Than Matcher Tests")
    {
        SECTION("two equal ints are not less than")
        {
            ASSERT(IsFalse, stf::LessThan(4).Matches(4));
        }

        SECTION("LHS int is less than RHS int, succeeds")
        {
            ASSERT(IsTrue, stf::LessThan(5).Matches(4));
        }

        SECTION("LHS int is greater than RHS int, fails")
        {
            ASSERT(IsFalse, stf::LessThan(4).Matches(5));
        }

        SECTION("two equal float4s are not less than")
        {
            ASSERT(IsFalse, stf::LessThan(float4(1.0, 2.0, 3.0, 4.0)).Matches(float4(1.0, 2.0, 3.0, 4.0)));
        }

        SECTION("LHS float4 is less than RHS float4, succeeds")
        {
            ASSERT(IsTrue, stf::LessThan(float4(1.0, 2.0, 3.0, 4.5)).Matches(float4(-1.0, -2.0, -3.0, -4.0)));
        }

        SECTION("LHS float4 is greater than RHS float4, fails")
        {
            ASSERT(IsFalse, stf::LessThan(float4(1.0, 2.0, 3.0, 4.5)).Matches(float4(10.0, 20.0, 30.0, 40.0)));
        }

        SECTION("two equal objects with operator< overload are not less than")
        {
            TestStruct left;
            TestStruct right;

            left.Value = 42;
            right.Value = 42;
            ASSERT(IsFalse, stf::LessThan(right).Matches(left));
        }

        SECTION("LHS object with operator< overload is less than RHS object, succeeds")
        {
            TestStruct left;
            TestStruct right;

            left.Value = 2;
            right.Value = 4;
            ASSERT(IsTrue, stf::LessThan(right).Matches(left));
        }

        SECTION("LHS object with operator< overload is greater than RHS object, fails")
        {
            TestStruct left;
            TestStruct right;

            left.Value = 4;
            right.Value = 2;
            ASSERT(IsFalse, stf::LessThan(right).Matches(left));
        }
    }
}