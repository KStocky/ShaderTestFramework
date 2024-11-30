// "/Test" is the virtual shader directory for the framework
// This is defined by the framework itself.
#include "/Test/STF/ShaderTestFramework.hlsli"

struct MyType
{
    uint a;
    float64_t b;
    bool c;
};

namespace ttl
{
    // Specializing caster for converting a MyType to bool
    template<>
    struct caster<bool, MyType>
    {
        static bool cast(MyType In)
        {
            return In.a < 4;
        }
    };
}

[numthreads(1, 1, 1)]
void Test()
{
    MyType evalToTrue = (MyType)0;
    evalToTrue.a = 2;
    MyType evalToFalse = (MyType)0;
    evalToFalse.a = 5;

    // IsTrue and IsFalse will evaluate ttl::cast<bool>(Input)
    // ttl::cast will then use the user defined ttl::caster specialization to do the cast.
    ASSERT(IsTrue, evalToTrue);
    ASSERT(IsFalse, evalToFalse);
}