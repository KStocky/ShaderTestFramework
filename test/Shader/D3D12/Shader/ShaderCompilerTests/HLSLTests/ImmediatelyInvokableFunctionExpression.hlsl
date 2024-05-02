

[numthreads(1,1,1)]
void Main()
{
    bool result = struct IIFE { bool operator()(){ return true; } }();
}