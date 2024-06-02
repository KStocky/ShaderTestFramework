RWBuffer<float16_t> Buff;

float16_t InVal;
[numthreads(1,1,1)]
void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    Buff[DispatchThreadId.x] = 32.0h * InVal;
}