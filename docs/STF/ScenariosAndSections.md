[Reference](../ShaderTestFramework.md)

# Scenarios and Sections

**Contents**
1. [An Introduction](#an-introduction-to-scenarios-and-sections)
2. [Writing Optional\<T> in HLSL](#writing-optionalt-in-hlsl)
3. [Testing Optional\<T> without Scenarios And Sections](#testing-optionalt-without-scenarios-and-sections)
4. [Testing Optional\<T> with Scenarios And Sections](#testing-optionalt-with-scenarios-and-sections)<br>
    a. [Following the Execution Line by Line](#following-the-execution-line-by-line)<br>
    b. [An Explanation](#an-explanation)<br>
    c. [A note on Strings](#a-note-on-strings)<br>
5. [Tracking down failures on a Particular Thread](#tracking-down-failures-on-a-particular-thread)

## An Introduction to Scenarios and Sections

Just like [Catch2](https://github.com/catchorg/Catch2), STF is able to use scenarios and sections to define tests. Using Scenarios and Sections allows test writers to reuse test setup code in a simple to understand manner. Throughout this topic, we will be referring to ([Ex3_ScenariosAndSections](../../examples/Ex3_ScenariosAndSections)).

NOTE: All threads in a dispatch MUST have the same control flow through a SCENARIO. This is because there isn't support for logging section information on a per thread basis. STF assumes all threads will follow the set control flow through the test. 

e.g. It is not supported for half of your threads to go into one section while the other half of your threads skip that section and go into a different section.

## Writing Optional\<T> in HLSL

Before we can compare different methods of writing tests, we must first have some code to test. For the sake of this example we will be testing the following struct

```c++
template<typename T>
struct Optional
{
    T Data;
    bool IsValid;

    void Reset()
    {
        Data = (T)0;
        IsValid = false;
    }

    void Set(T In)
    {
        Data = In;
        IsValid = true;
    }

    T GetOrDefault(T InDefault)
    {
        if (IsValid)
        {
            return Data;
        }

        return InDefault;
    }
};
```

It is essentially a very basic implementation of an optional type such as [std::optional](https://en.cppreference.com/w/cpp/utility/optional) but in HLSL. We might use this to return a value from a function that might not be valid. The main thing about this basic class is that it is complex enough to write a somewhat substantial test suite for, which should help to demonstrate the usefullness of Scenarios and Sections. You can find the implementation of this class in [Optional.hlsli](../../examples/Ex3_ScenariosAndSections/ShaderCode/Optional.hlsli)

## Testing Optional\<T> without Scenarios And Sections

The following tests can be found in [OptionalTests.hlsl](../../examples/Ex3_ScenariosAndSections/ShaderCode/OptionalTests.hlsl).

The first test we might write for this class could be something like the following:

```c++
[numthreads(1, 1, 1)]
void GIVEN_InvalidOptional_WHEN_ValidityQueried_THEN_NotValid()
{
    Optional<int> opt;
    opt.Reset();

    ASSERT(IsFalse, opt.IsValid);
}
```

This test is written in the GIVEN_WHEN_THEN style. We first create an optional\<T>, then call reset on it, then assert that it is not valid. This works fine and passes when we run it. Now we know that Reset works as intended. Then next test we might want to write could be something like this:

```c++
[numthreads(1, 1, 1)]
void GIVEN_InvalidOptional_WHEN_GetOrDefaultCalled_THEN_DefaultReturned()
{
    Optional<int> opt;
    opt.Reset();

    const int expectedValue = 42;

    Assert(AreEqual, expectedValue, opt.GetOrDefault(expectedValue));
}
```

So now we are testing to make sure that GetOrDefault behaves as we expect for a reset optional. This again passes and works as expected. The code is also fairly simple. However, it introduces two slight annoyances.

1. We are repeating ourselves. The first two lines of both tests are the same. In this case it isn't that bad. It is just two lines of setup, but it could get worse...
2. As we now have two shader test functions we now need a way of running them both.

The second annoyance is quite simple to solve. We simply make the two tests be functions that we call from our entry function. So something like this:

```c++
void GIVEN_InvalidOptional_WHEN_ValidityQueried_THEN_NotValid()
{
    Optional<int> opt;
    opt.Reset();

    ASSERT(IsFalse, opt.IsValid);
}

void GIVEN_InvalidOptional_WHEN_GetOrDefaultCalled_THEN_DefaultReturned()
{
    Optional<int> opt;
    opt.Reset();

    const int expectedValue = 42;

    ASSERT(AreEqual, expectedValue, opt.GetOrDefault(expectedValue));
}

[numthreads(1, 1, 1)]
void OptionalTestsWithoutScenariosAndSections()
{
    GIVEN_InvalidOptional_WHEN_ValidityQueried_THEN_NotValid();
    GIVEN_InvalidOptional_WHEN_GetOrDefaultCalled_THEN_DefaultReturned();
}
```

This then leads to a method of removing the first annoyance, which is the duplicated code. We could change the above to the following instead:

```c++
void GIVEN_InvalidOptional_WHEN_ValidityQueried_THEN_NotValid(const Optional<int> InOpt)
{
    ASSERT(IsFalse, InOpt.IsValid);
}

void GIVEN_InvalidOptional_WHEN_GetOrDefaultCalled_THEN_DefaultReturned(const Optional<int> InOpt)
{
    const int expectedValue = 42;

    ASSERT(AreEqual, expectedValue, InOpt.GetOrDefault(expectedValue));
}

[numthreads(1, 1, 1)]
void OptionalTestsWithoutScenariosAndSections()
{
    Optional<int> opt;
    opt.Reset();

    GIVEN_InvalidOptional_WHEN_ValidityQueried_THEN_NotValid(opt);
    GIVEN_InvalidOptional_WHEN_GetOrDefaultCalled_THEN_DefaultReturned(opt);
}
```

And now we have solved our two issues. Yay! So now we can carry on and test when we set a value on the optional and we get these next two tests:

```c++
void GIVEN_InvalidOptional_WHEN_ValueSet_THEN_IsValid(Optional<int> InOpt)
{
    const int expectedValue = 42;

    InOpt.Set(expectedValue);
    ASSERT(IsTrue, InOpt.IsValid);
}

void GIVEN_InvalidOptional_WHEN_ValueSet_THEN_GetReturnsValue(Optional<int> InOpt)
{
    const int expectedValue = 42;
    const int defaultValue = 24;

    InOpt.Set(expectedValue);
    ASSERT( AreEqual, expectedValue, InOpt.GetOrDefault(defaultValue));
}

[numthreads(1, 1, 1)]
void OptionalTestsWithoutScenariosAndSections()
{
    Optional<int> opt;
    opt.Reset();

    GIVEN_InvalidOptional_WHEN_ValidityQueried_THEN_NotValid(opt);
    GIVEN_InvalidOptional_WHEN_GetOrDefaultCalled_THEN_DefaultReturned(opt);
    GIVEN_InvalidOptional_WHEN_ValueSet_THEN_IsValid(opt);
    GIVEN_InvalidOptional_WHEN_ValueSet_THEN_GetReturnsValue(opt);
}
```

These tests pass and now we have confidence that both Set and GetOrDefault value do the right thing. Awesome! But we have our 2nd annoyance back. There is duplicated code. In both of these tests we are calling set. We could do what we did before again and create an intermediate function which sets the value on the optional and then calls these two functions. However, this starts becoming a little unwieldy, and probably not worth it. We are starting sacrifice too much code readability for the sake of not wanting to repeat ourselves. The problem then starts to become even more pronounced when we think about testing the behaviour of calling Reset after calling Set. That would be a third level of functions to call.

It is this problem that Scenarios and Sections can help with.

## Testing Optional\<T> with Scenarios And Sections

As we saw in [Testing Optional\<T> without Scenarios And Sections](#testing-optionalt-without-scenarios-and-sections) trying to write test code that does not involve repeating ourselves can lead to tests that are hard to read and scattered in terms of their placement in test files. Shader Test Framework provides Scenarios and Sections as an alternative way of structuring tests, with an aim of improving readability and reducing code repitition. Below is how we might write the tests shown in the previous section except with using scenarios and sections.

```c++
[numthreads(1, 1, 1)]
void OptionalTestsWithScenariosAndSections()
{
    SCENARIO("GIVEN An Optional that is reset")
    {
        Optional<int> opt;
        opt.Reset();

        SECTION("THEN IsValid returns false")
        {
            ASSERT(IsFalse, opt.IsValid);
        }

        SECTION("THEN GetOrDefault returns default value")
        {
            const int expectedValue = 42;
            ASSERT(AreEqual, expectedValue, opt.GetOrDefault(expectedValue));
        }

        SECTION("WHEN value is set")
        {
            const int expectedValue = 42;
            opt.Set(expectedValue);

            SECTION("THEN IsValid returns true")
            {
                ASSERT(IsTrue, opt.IsValid);
            }

            SECTION("THEN GetOrDefault returns set value")
            {
                const int defaultValue = 24;
                ASSERT(AreEqual, expectedValue, opt.GetOrDefault(defaultValue));
            }
        }
    }
}
```

### Following the Execution Line by Line

If you have not seen testing frameworks that structure their tests like this, the above code may look odd. So let's talk about it by following the execution of the test.

First, execution enters the `SCENARIO` block. The first thing to notice here is that we have a string that describes the "scenario". This in itself is odd because HLSL does not technically support strings.

Next we set up our `Optional` and call reset on it. Then we enter the first `SECTION` block. Sections also take a string to describe what is going on in it, just like `SCENARIO`s This will then assert that the 'Optional' is invalid. At this point we have executed the first test that we created in the previous section. 

Execution now leaves the first `SECTION` block and will skip every subsequent `SECTION`. It will then loop back to the start of the `SCENARIO` and check if every `SECTION` has been executed. Only one `SECTION` has been executed, so execution re-enters the `SCENARIO` from the top. This means that a new `Optional` is created and subsequently reset.

Next, we reach the first `SECTION` again, however, unlike the previous iteration, execution does not enter this `SECTION`. Instead, execution moves on to, and enters the second `SECTION`. At which point the assertion for the return value of `GetOrDefault` is called. Our second test case has now been performed. Now, execution leaves this `SECTION` and skips all future sections and ends up back at the `SCENARIO` check.

There are still `SECTION`s to be executed, so execution loops again. On this iteration of the loop the third `SECTION` is to be entered. This `SECTION` is different to the two previous `SECTION`s as it has two nested `SECTION`s. At this point the `Optional` will be set with the `expectedValue` and then execution reaches the first of the nested `SECTION`s, and enters it. The assert is called. That is the third of our test cases performed. Execution now continues and skips the last `SECTION`.

Once again, the check at `SCENARIO` is hit and once again, there are still `SECTION`s to be executed. Execution reaches the 3rd `SECTION` and enters it. Execution is able to enter this block because there is still one nested `SECTION` that needs to be executed. `Set` is called on the `Optional`, then the first nested `SECTION` is skipped, and the second nested `SECTION` is entered. Once this block has been executed, all of our test cases have been performed.

Execution now goes back to the `SCENARIO` check one last time. As all `SECTION`s have now been executed, this check fails and so execution now leaves the scope the `SCENARIO`. The tests are over.

### An Explanation

For each `SECTION`, the `SCENARIO` is executed from the start. This means that the three top level `SECTION`s are entered with a freshly reset `Optional`. You can think of writing tests like this as constructing a tree of code blocks. `SCENARIO` is the root node, and each `SECTION` is a child node. And to execute the tests, each node is visited as many times as required to visit every leaf node in a depth-first manner, starting from the root node.

This results in a series of tests where common setup is shared and tests can be read from top to bottom. This solves both annoyances that were introduced in the previous section.

1. Code repitition is kept to a minimum (or as it is in this example, completely removed)
2. Multiple test cases can be written in one shader entry function 

It also ensures that all code is kept together and is structured in a way, that is easy to reason about. This was the problem that became apparent towards the end of the last section when we were trying to structure our tests without Scenarios and Sections.

As previously mentioned, this mechanism is taken from [Catch2](https://github.com/catchorg/Catch2/) and more information on the subject can be read on [Catch2's tutorial](https://github.com/catchorg/Catch2/blob/devel/docs/tutorial.md#test-cases-and-sections).

### A note on Strings

As noted in [Following the Execution Line by Line](#following-the-execution-line-by-line), `SCENARIO` and `SECTION` take a string as a parameter. This is used in error reporting to better describe where a failed assert comes from. It also gives a nice fluent way of describing test cases. There are a couple of things to note about strings in HLSL:

1. Strings are not actually supported by HLSL. They only work due to exploiting DXC. You can read more about how this works in [Shader Printf in HLSL and DX12](https://therealmjp.github.io/posts/hlsl-printf/) by MJP. The implementation used in STF is based on this article. And it is a fantastic article on the topic. Definitely worth a read.

2. Unlike MJPs implementation there is no hard limit of 64 characters, and there is no requirement to run without debug optimizations. Turns out that the issue with the validator during compilation only occurs when processing strings in a dynamic loop. TTL, instead uses macros to produce an unrolled loop. Currently TTL supports up to 256 characters but this can easily be increased by increasing the size of the unrolled loop in [ttl::string](../../src/Shader/TTL/string.hlsli). [Link](https://discord.com/channels/590611987420020747/1209044520285507634) to a discord thread talking about the issues that MJPs implementation leads to.

## Tracking down failures on a Particular Thread

Code that we run on the GPU is naturally going to be multi-threaded. Most examples for this framework will use single threaded compute shaders for the sake of simplicity, however, it is not realistic to expect tests to always be written in this way. So if we change our previous test suite to run on 32 threads in a thread group like so:

```c++
[numthreads(32, 1, 1)]
void OptionalTestsWithScenariosAndSectionsAndThreadIds(uint3 DTid : SV_DispatchThreadID)
{
    SCENARIO("GIVEN An Optional that is reset")
    {
        Optional<int> opt;
        opt.Reset();

        SECTION("THEN IsValid returns false")
        {
            if (DTid.x == 16)
            {
                ASSERT(IsTrue, opt.IsValid);
            }
            else
            {
                ASSERT(IsFalse, opt.IsValid);
            }
        }

        SECTION("THEN GetOrDefault returns default value")
        {
            const int expectedValue = 42;
            ASSERT(AreEqual, expectedValue, opt.GetOrDefault(expectedValue));
        }

        SECTION("WHEN value is set")
        {
            const int expectedValue = 42;
            opt.Set(expectedValue);

            SECTION("THEN IsValid returns true")
            {
                ASSERT(IsTrue, opt.IsValid);
            }

            SECTION("THEN GetOrDefault returns set value")
            {
                const int defaultValue = 24;
                ASSERT(AreEqual, expectedValue, opt.GetOrDefault(defaultValue));
            }
        }
    }
}
```

we will get the following error:

```
FAILED:
  REQUIRE( fixture.RunTest("OptionalTestsWithScenariosAndSectionsAndThreadIds", 1, 1, 1) )
with expansion:
  There were 127 successful asserts and 1 failed assertions
  Assert 0:
  SCENARIO: GIVEN An Optional that is reset
        SECTION: THEN IsValid returns false
                Data 1: false

===============================================================================
test cases: 3 | 2 passed | 1 failed
assertions: 3 | 2 passed | 1 failed
```

This error states that there were 127 successful asserts and 1 failed one. Trying to track down this in a real test could be a nightmare. Obviously in this case it is very easy to see which thread produced the assert. It is very clearly the thread with threadid (16,0,0). But let's assume that we don't know that for the sake of this example. Shader Test Framework makes tracking down issues like this where an assert only fails on one thread out of many quite easy. STF provides a function that can be called at the start of a test called `stf::RegisterThreadID`. Passing the thread id to this function will provide the framework with ability to tag each failed assert with its associated thread id. So we can change the start of our test to the following:

```c++
[numthreads(32, 1, 1)]
void OptionalTestsWithScenariosAndSectionsAndThreadIds(uint3 DTid : SV_DispatchThreadID)
{
    stf::RegisterThreadID(DTid);
    SCENARIO("GIVEN An Optional that is reset")
    {
        // Everything else is the same
    }
}
```
This will change our assert error message to the following
```
FAILED:
  REQUIRE( fixture.RunTest("OptionalTestsWithScenariosAndSectionsAndThreadIds", 1, 1, 1) )
with expansion:
  There were 127 successful asserts and 1 failed assertions
  Assert 0:  ThreadId: (16, 0, 0)
  SCENARIO: GIVEN An Optional that is reset
        SECTION: THEN IsValid returns false
                Data 1: false
```

As you can see, the thread id is now given. This can be an exceptionally effective way of tracking down edge cases in certain test failures, and also in tests that are written as stress tests of a system.

---

[Top](#scenarios-and-sections)