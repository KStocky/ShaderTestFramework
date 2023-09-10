#include "ShaderTestFixture.h"
#include "D3D12AgilityDefinitions.h"

int main()
{
    ShaderTestFixture test;

    return test.IsValid() ? 1 : 0;
}