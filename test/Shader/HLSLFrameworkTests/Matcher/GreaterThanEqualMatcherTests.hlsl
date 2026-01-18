#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/STF/Matcher.hlsli"


struct TestStruct
{
    int Value;
    bool operator>=(TestStruct InOther)
    {
        return Value >= InOther.Value;
    }
};

[numthreads(1,1,1)]
void GreaterThanEqualMatcherTests()
{
    SCENARIO("Greater Than Equal Matcher Tests")
    {
        SECTION("two equal ints are Greater Than Equal")
        {
            ASSERT(IsTrue, stf::GreaterThanEqual(4).Matches(4));
        }

        SECTION("LHS int is less than RHS int, fails")
        {
            ASSERT(IsFalse, stf::GreaterThanEqual(5).Matches(4));
        }

        SECTION("LHS int is greater than RHS int, succeeds")
        {
            ASSERT(IsTrue, stf::GreaterThanEqual(4).Matches(5));
        }

        SECTION("two equal float4s are Greater Than Equal")
        {
            ASSERT(IsTrue, stf::GreaterThanEqual(float4(1.0, 2.0, 3.0, 4.0)).Matches(float4(1.0, 2.0, 3.0, 4.0)));
        }

        SECTION("LHS float4 is less than RHS float4, fails")
        {
            ASSERT(IsFalse, stf::GreaterThanEqual(float4(1.0, 2.0, 3.0, 4.5)).Matches(float4(-1.0, -2.0, -3.0, -4.0)));
        }

        SECTION("LHS float4 is greater than RHS float4, succeeds")
        {
            ASSERT(IsTrue, stf::GreaterThanEqual(float4(1.0, 2.0, 3.0, 4.5)).Matches(float4(10.0, 20.0, 30.0, 40.0)));
        }

        SECTION("two equal objects with operator>= overload are Greater Than Equal")
        {
            TestStruct left;
            TestStruct right;

            left.Value = 42;
            right.Value = 42;
            ASSERT(IsTrue, stf::GreaterThanEqual(right).Matches(left));
        }

        SECTION("LHS object with operator>= overload is less than RHS object, fails")
        {
            TestStruct left;
            TestStruct right;

            left.Value = 2;
            right.Value = 4;
            ASSERT(IsFalse, stf::GreaterThanEqual(right).Matches(left));
        }

        SECTION("LHS object with operator>= overload is greater than RHS object, succeeds")
        {
            TestStruct left;
            TestStruct right;

            left.Value = 4;
            right.Value = 2;
            ASSERT(IsTrue, stf::GreaterThanEqual(right).Matches(left));
        }
    }
}