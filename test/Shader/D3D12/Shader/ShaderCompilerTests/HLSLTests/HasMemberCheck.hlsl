struct TestStruct
{
    int A;

    void Func(){}
};

template<typename T>                                 
struct HasA 
{                                                       
    using Yes = int;
    using No = int[2];                                             
    template <typename U> struct TypeCheck;                     
    template <typename _1> static Yes chk(TypeCheck<_1::A>); 
    template <typename   > static No  chk(...);                    
    static bool const value = sizeof(chk<T>(0)) == sizeof(Yes);     
};

[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    bool result = HasA<TestStruct>::value;
}