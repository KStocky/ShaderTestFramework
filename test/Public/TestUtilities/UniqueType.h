
namespace stf
{
    template<typename T, auto = [] {} >
    struct UniqueType : T {};
}