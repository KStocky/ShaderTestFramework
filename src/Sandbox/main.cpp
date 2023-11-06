#include "Framework/ShaderTestFixture.h"

int main()
{
    ShaderTestFixture test{ {} };

    return test.IsValid() ? 1 : 0;
}