
namespace MyNamespace
{
    struct A
    {
        int data1;
        int data2;
    };
    
    A NamespacedStructBinding;
    
    int NamespacedValueBinding;
}

struct MyCBufferStruct
{
    int3 data1;
    int4 data2;
    Texture2D<int> tex1D;
};

ConstantBuffer<MyCBufferStruct> MyConstantBuffer : register(b0);

int ValueBinding;
float NotUsed;

RWBuffer<int> BuffArray[2];

StructuredBuffer<MyNamespace::A> MyStructBuffer;

cbuffer MyClassicBuffer : register(b6, space3)
{
    int data1;
}

[numthreads(1, 1, 1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    BuffArray[0][DispatchThreadId.x] = MyNamespace::NamespacedStructBinding.data1 + MyNamespace::NamespacedStructBinding.data2 * MyNamespace::NamespacedValueBinding;
    BuffArray[1][DispatchThreadId.x] = 
        MyConstantBuffer.data1.x * 
        MyConstantBuffer.data2.w + 
        ValueBinding + 
        MyStructBuffer[DispatchThreadId.x].data1 + 
        data1 + 
        MyConstantBuffer.tex1D.Load(int3(0, 0, 0)).r;
}