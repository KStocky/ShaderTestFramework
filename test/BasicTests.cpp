#include <CppUnitTest.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ShaderTestFrameworkTests
{
    TEST_CLASS(BasicTests) {
    public:
        TEST_METHOD(This_Should_Succeed) { Assert::AreEqual(0, 0); }
        TEST_METHOD(This_Should_Fail) { Assert::AreEqual(0, 42); }
    };
}