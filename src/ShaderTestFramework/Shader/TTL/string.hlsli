#ifndef TTL_STRING_HEADER
#define TTL_STRING_HEADER

#include "/Test/TTL/byte_writer.hlsli"
#include "/Test/TTL/macro.hlsli"
#include "/Test/TTL/memory.hlsli"
#include "/Test/TTL/type_traits.hlsli"

namespace ttl
{
    template<uint InMaxChars>
    struct string
    {
        static const uint MaxNumChars = InMaxChars;
        uint Data[(MaxNumChars + sizeof(uint) - 1) / sizeof(uint)];
        uint Size;

        void append(uint InChar)
        {
            if (Size < MaxNumChars)
            {
                const uint index = Size / sizeof(uint);
                const uint offset = (Size % sizeof(uint)) * 8;

                Data[index] |= ((InChar & 0xFF) << offset);
                ++Size;
            }
        }
    };
    
    template<uint N>
    struct byte_writer<string<N> >
    {
        static const bool has_writer = true;

        static uint bytes_required(string<N> In)
        {
            return ttl::aligned_offset(In.Size, 4u);
        }

        static uint alignment_required(string<N> In)
        {
            return ttl::align_of<uint>::value;
        }

        template<typename U>
        static void write(inout container_wrapper<U> InContainer, const uint InIndex, const string<N> In)
        {
            const uint size = bytes_required(In) / ttl::size_of<uint>::value;
            static const bool isByteAddress = ttl::container_traits<U>::is_byte_address;
            static const uint storeIndexModifier = isByteAddress ? 4 : 1;
            for (uint i = 0; i < size; ++i)
            {
                InContainer.store(InIndex + i * storeIndexModifier, In.Data[i]);
            }
        }
    };
}

namespace ttl_detail
{
    #define CHAR_CHECK(InCharA, InCharB, Ret) if (InCharA == InCharB) return Ret

    template<typename T>
    uint char_to_uint(T InChar)
    {
        CHAR_CHECK(InChar, '\0', 0);
        CHAR_CHECK(InChar, '\t', 9);
        CHAR_CHECK(InChar, '\n', 10);
        CHAR_CHECK(InChar, ' ', 32);
        CHAR_CHECK(InChar, '!', 33);
        CHAR_CHECK(InChar, '"', 34);
        CHAR_CHECK(InChar, '#', 35);
        CHAR_CHECK(InChar, '$', 36);
        CHAR_CHECK(InChar, '%', 37);
        CHAR_CHECK(InChar, '&', 38);

        CHAR_CHECK(InChar, '(', 40);
        CHAR_CHECK(InChar, ')', 41);
        CHAR_CHECK(InChar, '*', 42);
        CHAR_CHECK(InChar, '+', 43);

        CHAR_CHECK(InChar, '-', 45);
        CHAR_CHECK(InChar, '.', 46);
        CHAR_CHECK(InChar, '/', 47);
        CHAR_CHECK(InChar, '0', 48);
        CHAR_CHECK(InChar, '1', 49);
        CHAR_CHECK(InChar, '2', 50);
        CHAR_CHECK(InChar, '3', 51);
        CHAR_CHECK(InChar, '4', 52);
        CHAR_CHECK(InChar, '5', 53);
        CHAR_CHECK(InChar, '6', 54);
        CHAR_CHECK(InChar, '7', 55);
        CHAR_CHECK(InChar, '8', 56);
        CHAR_CHECK(InChar, '9', 57);
        CHAR_CHECK(InChar, ':', 58);
        CHAR_CHECK(InChar, ';', 59);
        CHAR_CHECK(InChar, '<', 60);
        CHAR_CHECK(InChar, '=', 61);
        CHAR_CHECK(InChar, '>', 62);
        CHAR_CHECK(InChar, '?', 63);
        CHAR_CHECK(InChar, '@', 64);
        CHAR_CHECK(InChar, 'A', 65);
        CHAR_CHECK(InChar, 'B', 66);
        CHAR_CHECK(InChar, 'C', 67);
        CHAR_CHECK(InChar, 'D', 68);
        CHAR_CHECK(InChar, 'E', 69);
        CHAR_CHECK(InChar, 'F', 70);
        CHAR_CHECK(InChar, 'G', 71);
        CHAR_CHECK(InChar, 'H', 72);
        CHAR_CHECK(InChar, 'I', 73);
        CHAR_CHECK(InChar, 'J', 74);
        CHAR_CHECK(InChar, 'K', 75);
        CHAR_CHECK(InChar, 'L', 76);
        CHAR_CHECK(InChar, 'M', 77);
        CHAR_CHECK(InChar, 'N', 78);
        CHAR_CHECK(InChar, 'O', 79);
        CHAR_CHECK(InChar, 'P', 80);
        CHAR_CHECK(InChar, 'Q', 81);
        CHAR_CHECK(InChar, 'R', 82);
        CHAR_CHECK(InChar, 'S', 83);
        CHAR_CHECK(InChar, 'T', 84);
        CHAR_CHECK(InChar, 'U', 85);
        CHAR_CHECK(InChar, 'V', 86);
        CHAR_CHECK(InChar, 'W', 87);
        CHAR_CHECK(InChar, 'X', 88);
        CHAR_CHECK(InChar, 'Y', 89);
        CHAR_CHECK(InChar, 'Z', 90);
        CHAR_CHECK(InChar, '[', 91);
        CHAR_CHECK(InChar, '\\', 92);
        CHAR_CHECK(InChar, ']', 93);
        CHAR_CHECK(InChar, '^', 94);
        CHAR_CHECK(InChar, '_', 95);
        CHAR_CHECK(InChar, '`', 96);
        CHAR_CHECK(InChar, 'a', 97);
        CHAR_CHECK(InChar, 'b', 98);
        CHAR_CHECK(InChar, 'c', 99);
        CHAR_CHECK(InChar, 'd', 100);
        CHAR_CHECK(InChar, 'e', 101);
        CHAR_CHECK(InChar, 'f', 102);
        CHAR_CHECK(InChar, 'g', 103);
        CHAR_CHECK(InChar, 'h', 104);
        CHAR_CHECK(InChar, 'i', 105);
        CHAR_CHECK(InChar, 'j', 106);
        CHAR_CHECK(InChar, 'k', 107);
        CHAR_CHECK(InChar, 'l', 108);
        CHAR_CHECK(InChar, 'm', 109);
        CHAR_CHECK(InChar, 'n', 110);
        CHAR_CHECK(InChar, 'o', 111);
        CHAR_CHECK(InChar, 'p', 112);
        CHAR_CHECK(InChar, 'q', 113);
        CHAR_CHECK(InChar, 'r', 114);
        CHAR_CHECK(InChar, 's', 115);
        CHAR_CHECK(InChar, 't', 116);
        CHAR_CHECK(InChar, 'u', 117);
        CHAR_CHECK(InChar, 'v', 118);
        CHAR_CHECK(InChar, 'w', 119);
        CHAR_CHECK(InChar, 'x', 120);
        CHAR_CHECK(InChar, 'y', 121);
        CHAR_CHECK(InChar, 'z', 122);
        CHAR_CHECK(InChar, '{', 123);
        CHAR_CHECK(InChar, '|', 124);
        CHAR_CHECK(InChar, '}', 125);
        CHAR_CHECK(InChar, '~', 126);

        return 255;
    }
    #undef CHAR_CHECK
}

#define TTL_CHAR_STAMP(i, InLength, InStr, OutStr) if (i >= InLength) break; OutStr.append(ttl_detail::char_to_uint(InStr[i]));
#define TTL_CHAR_STAMPER(InStamper, InN, InLength, InStr, OutStr) InStamper(0, TTL_CHAR_STAMP, InLength, InStr, OutStr)

#ifndef TTL_STRING_MAX_LENGTH
#define TTL_STRING_MAX_LENGTH 64
#endif

#ifndef TTL_ENABLE_STRINGS
#define TTL_ENABLE_STRINGS 0
#endif

#if TTL_ENABLE_STRINGS
#define CREATE_STRING(OutStr, InStrLiteral)                                                                                          \
_Static_assert((__decltype(ttl::array_len(InStrLiteral))::value <= TTL_STRING_MAX_LENGTH), "Strings with greater than " TTL_STRINGIFY(TTL_STRING_MAX_LENGTH) " characters are not supported"); \
ttl::string<__decltype(ttl::array_len(InStrLiteral))::value> OutStr;                                                                 \
ttl::zero(OutStr);                                                                                                                   \
do {                                                                                                                                 \
    using LengthType = __decltype(ttl::array_len(InStrLiteral));                                                                     \
    TTL_STAMP(TTL_STRING_MAX_LENGTH, TTL_CHAR_STAMPER, LengthType::value, InStrLiteral, OutStr)                                      \
} while(false)

#else

#define CREATE_STRING(OutStr, InStrLiteral)                                                                                          \
ttl::string<__decltype(ttl::array_len(InStrLiteral))::value> OutStr;                                                                 \
ttl::zero(OutStr);                                                                                                                   

#endif

#endif