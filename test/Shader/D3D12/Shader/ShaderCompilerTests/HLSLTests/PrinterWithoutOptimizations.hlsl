
template<typename T, uint N>
uint ArrayLen(T In[N])
{
    return N;
}

#define CHAR_CHECK(InCharA, InCharB, Ret) if (InCharA == InCharB) return Ret

template<typename T>
uint CharToUintASCII(T InChar)
{
    CHAR_CHECK(InChar, '\0', 0);
    CHAR_CHECK(InChar, '\t', 9);
    CHAR_CHECK(InChar, '\n', 10);
    CHAR_CHECK(InChar, ' ', 32);
    CHAR_CHECK(InChar, '!', 33);
    CHAR_CHECK(InChar, '\"', 33);
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

struct StringBufferPacked
{
    static const uint MaxNumChars = 256;
    uint Data[MaxNumChars / sizeof(uint)];
    uint Size;

    void AppendChar(uint InChar)
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

struct StringBufferUnpacked
{
    static const uint MaxNumChars = 256;
    uint Data[MaxNumChars];
    uint Size;

    void AppendChar(uint InChar)
    {
        if (Size < MaxNumChars)
        {
            const uint index = Size;

            Data[index] = InChar;
            ++Size;
        }
    }
};

#if OPTIMIZATIONS_ENABLED
#define TO_STRING_BUFFER(InType, InBuffer, InStr)             \
do{                                                       \
    const uint numChars = ArrayLen(InStr);              \
    InBuffer = (InType)0;                                     \
    for (uint i = 0; i < numChars; ++i)                 \
    {                                                   \
        InBuffer.AppendChar(CharToUintASCII(InStr[i]));      \
    }                                                   \
}while(false)
#else
#define TO_STRING_BUFFER(InType, InBuffer, InStr) InBuffer = (InType)0;
#endif

RWBuffer<uint> output;

[numthreads(1,1,1)]
void Main()
{
    StringBufferPacked actual;
    TO_STRING_BUFFER(StringBufferPacked, actual, "Hello");

    output[0] = actual.Size;
    output[1] = actual.Data[0];

}