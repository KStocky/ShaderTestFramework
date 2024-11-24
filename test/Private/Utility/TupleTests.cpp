
#include "Utility/Tuple.h"

namespace TupleTests
{
#if _MSC_VER >= 1942
    static_assert(sizeof(tuplet::tuple<bool, int>) == 4, "When this fails because it returns 8, that means that https://developercommunity.visualstudio.com/t/Unexpected-behaviour-with-msvc::no_uniqu/10798165 has been fixed. In that case we can go back to using tuplet::tuple instead of std::tuple");
#endif
}