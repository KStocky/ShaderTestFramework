#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE_PERSISTENT_FIXTURE(ShaderTestFixtureBaseFixture, "HLSLFrameworkTests - ProofOfConcept")
{
    using namespace stf;
    auto testName = GENERATE
    (
        "GIVEN_TwoCallsToCounter_WHEN_Compared_THEN_AreDifferent",
        "GIVEN_StaticGlobalArray_WHEN_Inspected_THEN_AllZeroed",
        "SectionTest",
        "GIVEN_TwoDifferentSizedStructs_WHEN_sizeofCalledOn_Them_THEN_CorrectSizeReported",
        "GIVEN_SomeTypesWithAndWithoutASpecializations_WHEN_ApplyFuncCalledOnThem_THEN_ExpectedResultsReturned",
        "StringsAndTemplates",
        // This no longer compiles because you can't get the length of a string at compile time in local scope anymore
        // Leaving it here just for reference
        //"MatesPrinter",
        "VariadicMacroOverloading"
    );

    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(
            AssertionsV1::ShaderTestFixture::RuntimeTestDesc
            {
                .CompilationEnv
                {
                    .Source = fs::path(std::format("/Tests/ProofOfConcept/{}.hlsl", testName))
                },
                .TestName = testName,
                .ThreadGroupCount{1, 1, 1}
            })
        );
    }
}

TEST_CASE_PERSISTENT_FIXTURE(ShaderTestFixtureBaseFixture, "HLSLFrameworkTests - Macro-expanded include")
{
    using namespace stf;

    REQUIRE(fixture.RunTest(
        AssertionsV1::ShaderTestFixture::RuntimeTestDesc
        {
            .CompilationEnv
            {
                .Source = std::string
                {
                    R"(
                        #include STF_ASSERTION_INTERFACE_HEADER

                        [numthreads(1, 1, 1)]
                        void MacroExpandedInclude()
                        {
                            ASSERT(AreEqual, 1, 1);
                        }
                    )"
                },
                .Defines
                {
                    ShaderMacro
                    {
                        .Name = "STF_ASSERTION_INTERFACE_HEADER",
                        .Definition = R"("/Test/STF/ShaderTestFramework.hlsli")"
                    }
                }
            },
            .TestName = "MacroExpandedInclude",
            .ThreadGroupCount{ 1, 1, 1 }
        })
    );
}
