[Reference](../ShaderTestFramework.md)

# Asserts

All code snippets for this document will be taken from ([Ex4_Asserts](../../examples/Ex4_Asserts))

**Contents**
1. [Assertions Provided](#assertions-provided)
2. [The ASSERT Macro](#the-assert-macro)

## Assertions Provided

Shader Test Framework currently provides the following assertion functions in the `STF` namespace

1. `STF::IsTrue` - Takes a single argument. If it evaluates to true, the assertion passes
2. `STF::IsFalse` - Takes a single argument. If it evaluates to false, the assertion passes
3. `STF::AreEqual` - Takes two arguments of the EXACT same type. If calling `operator==` on these two arguments evaluates to true, the assertion passes
4. `STF::NotEqual` - Takes two arguments of the EXACT same type. If calling `operator==` on these two arguments evaluates to false, the assertion passes
5. `STF::Fail` - Takes no arguments. Calling this will instantly add a failing assertion.

The first 4 of these assertions can be used by any vector form of uint, int, float and bool. e.g. `STF::AreEqual(uint3(1,2,3), uint(1,2,3));` would compile and succeed. `STF::AreEqual(uint3(2,2,3), uint(1,2,3));` would compile and fail.

## The ASSERT Macro

To improve the provided assertion functions, Shader Test Framework, also provides the `ASSERT` macro. This macro can be used with any of the above assertion functions. e.g. you can either write `STF::IsTrue(condition)` OR `ASSERT(IsTrue, condition)` to assert that a variable called `condition` is true. To demonstrate the usefulness of this macro, let's have a look at running the tests in ([AssertTests.hlsl](../../examples/Ex4_Asserts/ShaderCode/AssertTests.hlsl)). If only the `FailingStandardAsserts` are run:

```c++
[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void AssertTests()
{
    //PassingStandardAsserts();
    FailingStandardAsserts();
    //PassingAssertsWithMacro();
    //FailingAssertsWithMacro();
    //FailingStandardAssertsWithIds();
}
```

we will get a failing assert report like the following:

```
FAILED:
  REQUIRE( fixture.RunTest("AssertTests", 1, 1, 1) )
with expansion:
  There were 0 successful asserts and 4 failed assertions
  Assert 0:
  Data 1: false
  Assert 1:
  Data 1: true
  Assert 2:
  Data 1: 42
  Data 2: 24
  Assert 3:
  Data 1: 42
  Data 2: 42

===============================================================================
test cases: 1 | 1 failed
assertions: 1 | 1 failed
```

In this case, this is enough information to identify the failing asserts. However, in a more complex testing scenario, this is likely not enough to be able to pinpoint which assertions failed. To contrast this, if we instead run the `FailingAssertsWithMacro()` set of tests, then we will get the following results:

```
FAILED:
  REQUIRE( fixture.RunTest("AssertTests", 1, 1, 1) )
with expansion:
  There were 0 successful asserts and 4 failed assertions
  Assert 0: Line: 31
  Data 1: false
  Assert 1: Line: 32
  Data 1: true
  Assert 2: Line: 33
  Data 1: 42
  Data 2: 24
  Assert 3: Line: 34
  Data 1: 42
  Data 2: 42

===============================================================================
test cases: 1 | 1 failed
assertions: 1 | 1 failed
```

The difference here is that every assert now provides the line on which the assertion failed. Therefore making it much clearer what asserts failed.

---

[Top](#asserts)