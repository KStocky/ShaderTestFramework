
template<bool Value>
struct StaticAssert;

template<>
struct StaticAssert<true>{};

// The error message for this static assert is way too messy due to the multiple levels of macro expansion
#define STATIC_ASSERT_NAME(InId)  ASSERT_VAR##InId
#define STATIC_ASSERT_IMPL(Expression, InId) StaticAssert<!!Expression> STATIC_ASSERT_NAME(InId)
#define STATIC_ASSERT(Expression, Message) STATIC_ASSERT_IMPL(Expression, __COUNTER__)

// This is much better since there is much less macro expansion.
#define STATIC_ASSERT2(Expression, ...) ((int1)42)[!Expression]

// This version does not work at all
#define STATIC_ASSERT3(Expression, ...) struct { static const int Val = ((int1)42)[!Expression]; }


// This provides a similar error message to STATIC_ASSERT2 but it works in the global namespace.
#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define STATIC_ASSERT4(Expression, ...) static const int CONCAT(ASSERT_VAR_, __COUNTER__) = ((int1)42)[!Expression]

// Doesn't work in structs. Apparently this is fine because it is static const. Otherwise it would fail
// But then we couldn't use it globally because it would be seen as a shader input which is not ok.
#define STATIC_ASSERT5(Expression, ...) static const StaticAssert<!!Expression> CONCAT(ASSERT_VAR_, __COUNTER__)

// This just doesn't work whatsoever
#define STATIC_ASSERT6(Expression, ...) using CONCAT(ASSERT_TYPE_, __COUNTER__) = StaticAssert<!!Expression>

// This works!
#define STATIC_ASSERT7(Expression, ...) static const vector<int, !!(Expression)> CONCAT(ASSERT_TYPE_, __COUNTER__)

STATIC_ASSERT4(true);
STATIC_ASSERT4(true);

STATIC_ASSERT5(true);
STATIC_ASSERT6(false);

STATIC_ASSERT7(true);
//STATIC_ASSERT7(false);

struct A
{
    STATIC_ASSERT5(false);
    STATIC_ASSERT6(false);

    STATIC_ASSERT7(true);
    //STATIC_ASSERT7(false);
};

[numthreads(1,1,1)]
void Main()
{
    STATIC_ASSERT(true, "This should not fail");
    STATIC_ASSERT(true, "This also should not fail");
    //STATIC_ASSERT(false, "This fails");

    STATIC_ASSERT2(42 == 42);
    STATIC_ASSERT2(42 == 42, "This should not fail");
    STATIC_ASSERT2(true, "This also should not fail");
    //STATIC_ASSERT2(false, "This fails");
    //STATIC_ASSERT2(42 != 42, "This also should fail");
}