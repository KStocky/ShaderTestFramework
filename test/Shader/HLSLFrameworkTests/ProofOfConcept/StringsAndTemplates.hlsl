#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/ttl/static_assert.hlsli"

template<typename T, uint N>
uint ArrayLen(T In[N])
{
    return N;
}

template<typename T>
struct RemoveConst
{
    using Type = T;
};

template<typename T>
struct RemoveConst<const T>
{
    using Type = T;
};

STATIC_ASSERT((ttl::is_same<RemoveConst<const uint>::Type, uint>::value));

template<typename T, uint N>
struct MyString;

template<typename T, uint N>
struct MyString<const T, N>
{
    T Data[N];

    void Copy(inout MyString Dest)
    {
        for (int i = 0; i < N; ++i)
        {
            Dest.Data[i] = Data[i];
        }
    }

    bool operator==(MyString In)
    {
        for (int i = 0; i < N - 1; ++i)
        {
            if (Data[1] != In.Data[1])
            {
                return false;
            }
        }
        return true;
    }
};

template<typename T, uint N>
MyString<T, N> Create(T In[N])
{
    MyString<T, N> ret;
    ret.Data = In;
    return ret;
}

template<typename T>
bool IsE(T InChar)
{
    return InChar == 'e';
}

template<typename T, uint N>
void DoThings(T In[N])
{
    bool isEqual = IsE(In[1]);

    ASSERT(IsTrue, isEqual);
}

template<typename T>
uint CastToInt(T In)
{
    return (uint)In;
}

#define DO_THINGS(In)               \
{                                   \
    bool isEqual = IsE(In[1]);      \
    ASSERT(IsTrue, isEqual);        \
}                                   \

#define DO_THINGS1(In)              \
{                                   \
    uint val = CastToInt(In[1]);    \
    ASSERT(AreEqual, val, 90u);     \
} 

//#define CHARS "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890 ,./?;:'@#~[]{}-_=+!£$%^&*()\0\n\t\\"

#define CHARS_1 "QWERTYUIOPASDFGHJKLZXCVBNM"
#define CHARS_2 "qwertyuiopasdfghjklzxcvbnm"
#define CHARS_3 "1234567890 ,."
#define CHARS_4 "/?;:'@#~[]{}-_=+!£$%^&*()\n\t\\"

template<typename T>
uint CharToUintDivided(T InChar)
{
    const uint chars1Length = ArrayLen(CHARS_1);
    const uint chars2Length = ArrayLen(CHARS_2);
    const uint chars3Length = ArrayLen(CHARS_3);
    const uint chars4Length = ArrayLen(CHARS_4);
    const uint numPossibleCharacters = chars1Length + chars2Length + chars3Length + chars4Length;

    for (uint i = 0; i < chars1Length; ++i)
    {
        if (InChar == CHARS_1[i])
        {
            return i;
        }
    }

    for (uint i = 0; i < chars2Length; ++i)
    {
        if (InChar == CHARS_2[i])
        {
            return i + chars1Length;
        }
    }

    for (uint i = 0; i < chars3Length; ++i)
    {
        if (InChar == CHARS_3[i])
        {
            return i + chars1Length + chars2Length;
        }
    }

    for (uint i = 0; i < chars4Length; ++i)
    {
        if (InChar == CHARS_4[i])
        {
            return i + chars1Length + chars2Length + chars3Length;
        }
    }

    return numPossibleCharacters;
}

//template<typename T>
//uint CharToUint(T InChar)
//{
//    const uint numPossibleCharacters = ArrayLen(CHARS);
//
//    for (uint i = 0; i < numPossibleCharacters; ++i)
//    {
//        if (InChar == CHARS[i])
//        {
//            return i;
//        }
//    }
//
//    return numPossibleCharacters;
//}

#define CHAR_CHECK(InCharA, InCharB, Ret) if (InCharA == InCharB) return Ret

template<typename T>
uint CharToUintASCII(T InChar)
{
    CHAR_CHECK(InChar, '\0', 0);
    CHAR_CHECK(InChar, '\t', 9);
    CHAR_CHECK(InChar, '\n', 10);
    CHAR_CHECK(InChar, ' ', 32);
    CHAR_CHECK(InChar, '!', 33);
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


struct StringBuffer
{
    static const uint MaxNumChars = 256;
    uint Data[MaxNumChars];
    uint Size;

    void Init()
    {
        for (uint i = 0; i < MaxNumChars; ++i)
        {
            Data[i] = 0;
        }

        Size = 0;
    }

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

namespace ttl
{
    template<>
    struct caster<bool, StringBuffer>
    {
        static bool cast(StringBuffer In)
        {
            return false;
        }
    };
}

#define TO_STRING_BUFFER(InBuffer, InStr)               \
{                                                       \
    const uint numChars = ArrayLen(InStr);              \
    InBuffer = (StringBuffer)0;                         \
    for (uint i = 0; i < numChars; ++i)                 \
    {                                                   \
        InBuffer.AppendChar(CharToUint(InStr[i]));      \
    }                                                   \
}                                                       \

#define TO_STRING_BUFFER_2(InBuffer, InStr)             \
do{                                                       \
    const uint numChars = ArrayLen(InStr);              \
    InBuffer = (StringBuffer)0;                         \
    for (uint i = 0; i < numChars; ++i)                 \
    {                                                   \
        InBuffer.AppendChar(CharToUintDivided(InStr[i]));      \
    }                                                   \
}while(false)

#define TO_STRING_BUFFER_3(InBuffer, InStr)             \
do{                                                       \
    const uint numChars = ArrayLen(InStr);              \
    InBuffer.Init();                                     \
    for (uint i = 0; i < numChars; ++i)                 \
    {                                                   \
        InBuffer.AppendChar(CharToUintASCII(InStr[i]));      \
    }                                                   \
}while(false)

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void StringsAndTemplates()
{
    StringBuffer actual;
    TO_STRING_BUFFER_3(actual, "Hello There! I really hope that this works\n And that there are ");

    ASSERT(AreEqual, actual.Size, 64u);
    //ASSERT(IsTrue, actual);

    ASSERT(AreEqual, 72u, actual.Data[0]);
    ASSERT(AreEqual, 101u, actual.Data[1]);
    //ASSERT(AreEqual, 108u, actual.Data[2]);
    //ASSERT(AreEqual, 108u, actual.Data[3]);
    //ASSERT(AreEqual, 111u, actual.Data[4]);
    //ASSERT(AreEqual, 32u, actual.Data[5]);
    //ASSERT(AreEqual, 84u, actual.Data[6]);
    //ASSERT(AreEqual, 104u, actual.Data[7]);
    //ASSERT(AreEqual, 101u, actual.Data[8]);
    //ASSERT(AreEqual, 114u, actual.Data[9]);
    //ASSERT(AreEqual, 101u, actual.Data[10]);
    //ASSERT(AreEqual, 0u, actual.Data[11]);
}