
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