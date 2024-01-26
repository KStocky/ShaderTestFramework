template<bool InCond, typename T = void>
struct enable_if
{
};

template<typename T>
struct enable_if<true, T>
{
    using type = T;  
};

template<uint Val>
struct A
{

};

template<>
struct A<4>
{
    static const uint Val = 4;
};

template<uint InVal, typename = void>
struct HasVal
{
    static const bool Value = false;
};

template<uint InVal>
struct HasVal<InVal, typename enable_if<A<InVal>::Val != 0>::type>
{
    static const bool Value = true;
};

template<bool Value>
struct StaticAssert;

template<>
struct StaticAssert<true>{};

[numthreads(1,1,1)]
void Main()
{
    //Does not compile
    //StaticAssert<HasVal<3>::Value> test;

    //Compiles
    StaticAssert<HasVal<4>::Value> test;
}