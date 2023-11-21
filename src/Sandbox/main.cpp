
#include "Utility/Tuple.h"
#include <tuple>

#include <iostream>

struct MyType
{
    MyType()
    {
        std::cout << "Default\n";
    }

    MyType(const MyType&)
    {
        std::cout << "Copy\n";
    }

    MyType(MyType&&) noexcept
    {
        std::cout << "Move\n";
    }

    MyType& operator=(const MyType&)
    {
        std::cout << "Copy Assign\n";
    }

    MyType& operator=(MyType&&) noexcept
    {
        std::cout << "Move Assign\n";
    }

    ~MyType()
    {
        std::cout << "Destruct\n";
    }
};

#define NAMESPACE tuplet

NAMESPACE::tuple<MyType, MyType> GetTheThings() { return NAMESPACE::tuple{MyType(), MyType()}; }

int main()
{
    const auto [thing1, thing2] = GetTheThings();

    return 0;
}