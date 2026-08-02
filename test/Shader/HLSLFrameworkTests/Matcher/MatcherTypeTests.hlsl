
#include "/Test/STF/Matcher.hlsli"

namespace MatcherTypeTests
{
    struct LHSType{};

    struct NoMatchesFunction
    {
        int Value;
    };

    struct NoValueMember
    {
        bool Matches(LHSType) {return true;}
    };

    struct MatchesFuncTakesZeroArgs
    {
        bool Matches() {return true;}
        int Value;
    };

    struct MatchesFuncTakesTwoArgs
    {
        bool Matches(LHSType, LHSType) {return true;}
        int Value;
    };

    struct ValidMatcher
    {
        bool Matches(LHSType) {return true;}
        int Value;
    };

    _Static_assert(!ttl::models<stf::matcher_type, NoMatchesFunction, LHSType>::value);
    _Static_assert(!ttl::models<stf::matcher_type, NoValueMember, LHSType>::value);
    _Static_assert(!ttl::models<stf::matcher_type, MatchesFuncTakesZeroArgs, LHSType>::value);
    _Static_assert(!ttl::models<stf::matcher_type, MatchesFuncTakesTwoArgs, LHSType>::value);
    _Static_assert(ttl::models<stf::matcher_type, ValidMatcher, LHSType>::value);
}

namespace HasBoolBaseCompileTests
{
    struct NotFundamental{};
    _Static_assert(stf::detail::has_bool_base_v<bool>);
    _Static_assert(stf::detail::has_bool_base_v<bool2>);
    _Static_assert(stf::detail::has_bool_base_v<bool3>);
    _Static_assert(stf::detail::has_bool_base_v<bool4>);

    _Static_assert(stf::detail::has_bool_base_v<const bool>);
    _Static_assert(stf::detail::has_bool_base_v<const bool2>);
    _Static_assert(stf::detail::has_bool_base_v<const bool3>);
    _Static_assert(stf::detail::has_bool_base_v<const bool4>);

    _Static_assert(!stf::detail::has_bool_base_v<int>);
    _Static_assert(!stf::detail::has_bool_base_v<int2>);
    _Static_assert(!stf::detail::has_bool_base_v<int3>);
    _Static_assert(!stf::detail::has_bool_base_v<int4>);

    _Static_assert(!stf::detail::has_bool_base_v<NotFundamental>);
}

namespace EqualsComparableWithCompileTests
{
    struct RHSType{};
    struct LHSType
    {
        bool operator==(RHSType)
        {
            return true;
        }
    };

    struct A{};

    _Static_assert(ttl::models<stf::EqualComparableWith, int, int>::value);
    _Static_assert(ttl::models<stf::EqualComparableWith, int4, int4>::value);
    _Static_assert(ttl::models<stf::EqualComparableWith, int4x4, int4x4>::value);

    _Static_assert(ttl::models<stf::EqualComparableWith, LHSType, RHSType>::value);
    _Static_assert(!ttl::models<stf::EqualComparableWith, RHSType, LHSType>::value);
    _Static_assert(!ttl::models<stf::EqualComparableWith, A, A>::value);
}

namespace EqualsMatcherCompileTests
{
    struct RHSType{};
    struct LHSType
    {
        bool operator==(RHSType)
        {
            return true;
        }
    };

    struct A{};

    _Static_assert(ttl::models<stf::matcher_type, stf::EqualsMatcher<RHSType>, LHSType>::value);
    _Static_assert(ttl::models<stf::matcher_type, __decltype(stf::Equals(ttl::declval<RHSType>())), LHSType>::value);
    _Static_assert(!ttl::models<stf::matcher_type, stf::EqualsMatcher<A>, A>::value);
    _Static_assert(!ttl::models<stf::matcher_type, __decltype(stf::Equals(ttl::declval<A>())), A>::value);
}

namespace LessThanComparableWithCompileTests
{
    struct RHSType{};
    struct LHSType
    {
        bool operator<(RHSType)
        {
            return true;
        }
    };

    struct A{};

    _Static_assert(ttl::models<stf::LessThanComparableWith, int, int>::value);
    _Static_assert(ttl::models<stf::LessThanComparableWith, int4, int4>::value);
    _Static_assert(ttl::models<stf::LessThanComparableWith, int4x4, int4x4>::value);

    _Static_assert(ttl::models<stf::LessThanComparableWith, LHSType, RHSType>::value);
    _Static_assert(!ttl::models<stf::LessThanComparableWith, RHSType, LHSType>::value);
    _Static_assert(!ttl::models<stf::LessThanComparableWith, A, A>::value);
}

namespace LessThanMatcherCompileTests
{
    struct RHSType{};
    struct LHSType
    {
        bool operator<(RHSType)
        {
            return true;
        }
    };

    struct A{};

    _Static_assert(ttl::models<stf::matcher_type, stf::LessThanMatcher<RHSType>, LHSType>::value);
    _Static_assert(ttl::models<stf::matcher_type, __decltype(stf::LessThan(ttl::declval<RHSType>())), LHSType>::value);
    _Static_assert(!ttl::models<stf::matcher_type, stf::LessThanMatcher<A>, A>::value);
    _Static_assert(!ttl::models<stf::matcher_type, __decltype(stf::LessThan(ttl::declval<A>())), A>::value);
}

namespace LessThanEqualComparableWithCompileTests
{
    struct RHSType{};
    struct LHSType
    {
        bool operator<=(RHSType)
        {
            return true;
        }
    };

    struct A{};

    _Static_assert(ttl::models<stf::LessThanEqualComparableWith, int, int>::value);
    _Static_assert(ttl::models<stf::LessThanEqualComparableWith, int4, int4>::value);
    _Static_assert(ttl::models<stf::LessThanEqualComparableWith, int4x4, int4x4>::value);

    _Static_assert(ttl::models<stf::LessThanEqualComparableWith, LHSType, RHSType>::value);
    _Static_assert(!ttl::models<stf::LessThanEqualComparableWith, RHSType, LHSType>::value);
    _Static_assert(!ttl::models<stf::LessThanEqualComparableWith, A, A>::value);
}

namespace LessThanEqualMatcherCompileTests
{
    struct RHSType{};
    struct LHSType
    {
        bool operator<=(RHSType)
        {
            return true;
        }
    };

    struct A{};

    _Static_assert(ttl::models<stf::matcher_type, stf::LessThanEqualMatcher<RHSType>, LHSType>::value);
    _Static_assert(ttl::models<stf::matcher_type, __decltype(stf::LessThanEqual(ttl::declval<RHSType>())), LHSType>::value);
    _Static_assert(!ttl::models<stf::matcher_type, stf::LessThanEqualMatcher<A>, A>::value);
    _Static_assert(!ttl::models<stf::matcher_type, __decltype(stf::LessThanEqual(ttl::declval<A>())), A>::value);
}

namespace GreaterThanEqualComparableWithCompileTests
{
    struct RHSType{};
    struct LHSType
    {
        bool operator>=(RHSType)
        {
            return true;
        }
    };

    struct A{};

    _Static_assert(ttl::models<stf::GreaterThanEqualComparableWith, int, int>::value);
    _Static_assert(ttl::models<stf::GreaterThanEqualComparableWith, int4, int4>::value);
    _Static_assert(ttl::models<stf::GreaterThanEqualComparableWith, int4x4, int4x4>::value);

    _Static_assert(ttl::models<stf::GreaterThanEqualComparableWith, LHSType, RHSType>::value);
    _Static_assert(!ttl::models<stf::GreaterThanEqualComparableWith, RHSType, LHSType>::value);
    _Static_assert(!ttl::models<stf::GreaterThanEqualComparableWith, A, A>::value);
}

namespace GreaterThanEqualMatcherCompileTests
{
    struct RHSType{};
    struct LHSType
    {
        bool operator>=(RHSType)
        {
            return true;
        }
    };

    struct A{};

    _Static_assert(ttl::models<stf::matcher_type, stf::GreaterThanEqualMatcher<RHSType>, LHSType>::value);
    _Static_assert(ttl::models<stf::matcher_type, __decltype(stf::GreaterThanEqual(ttl::declval<RHSType>())), LHSType>::value);
    _Static_assert(!ttl::models<stf::matcher_type, stf::GreaterThanEqualMatcher<A>, A>::value);
    _Static_assert(!ttl::models<stf::matcher_type, __decltype(stf::GreaterThanEqual(ttl::declval<A>())), A>::value);
}

namespace GreaterThanComparableWithCompileTests
{
    struct RHSType{};
    struct LHSType
    {
        bool operator>(RHSType)
        {
            return true;
        }
    };

    struct A{};

    _Static_assert(ttl::models<stf::GreaterThanComparableWith, int, int>::value);
    _Static_assert(ttl::models<stf::GreaterThanComparableWith, int4, int4>::value);
    _Static_assert(ttl::models<stf::GreaterThanComparableWith, int4x4, int4x4>::value);

    _Static_assert(ttl::models<stf::GreaterThanComparableWith, LHSType, RHSType>::value);
    _Static_assert(!ttl::models<stf::GreaterThanComparableWith, RHSType, LHSType>::value);
    _Static_assert(!ttl::models<stf::GreaterThanComparableWith, A, A>::value);
}

namespace GreaterThanMatcherCompileTests
{
    struct RHSType{};
    struct LHSType
    {
        bool operator>(RHSType)
        {
            return true;
        }
    };

    struct A{};

    _Static_assert(ttl::models<stf::matcher_type, stf::GreaterThanMatcher<RHSType>, LHSType>::value);
    _Static_assert(ttl::models<stf::matcher_type, __decltype(stf::GreaterThan(ttl::declval<RHSType>())), LHSType>::value);
    _Static_assert(!ttl::models<stf::matcher_type, stf::GreaterThanMatcher<A>, A>::value);
    _Static_assert(!ttl::models<stf::matcher_type, __decltype(stf::GreaterThan(ttl::declval<A>())), A>::value);
}
