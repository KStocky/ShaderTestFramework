#pragma once
#include "/Test/TTL/type_traits.hlsli"

namespace ttl
{
    template<typename To, typename From, typename = void>
    struct caster;

    template<typename From>
    struct caster<bool, From, typename enable_if<fundamental_type_traits<From>::is_fundamental>::type>
    {
        static bool cast(From In)
        {
            return all(In);
        }
    };

    template<typename To, typename From>
    To cast(From In)
    {
        return caster<To, From>::cast(In);
    }
}