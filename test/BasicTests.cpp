#include <CppUnitTest.h>

#include "ShaderTestFixture.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ShaderTestFrameworkTests
{
    TEST_CLASS(BasicTests) {
    public:
        TEST_METHOD(This_Should_Succeed) 
        { 
            Assert::AreEqual(0, 0); 
        }
        TEST_METHOD(This_Should_Also_Succeed) { Assert::AreNotEqual(0, 42); }
    };

    TEST_CLASS(ShaderTestFixtureTests)
    {
        TEST_METHOD(This_Should_Succeed)
        {
            ShaderTestFixture Test;
            Assert::IsTrue(Test.IsValid());
        }
    };
}