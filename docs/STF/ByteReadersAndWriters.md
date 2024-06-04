[Reference](../ShaderTestFramework.md)

# Byte Readers and Writers

The HLSL and C++ sides of the framework need to be able to communicate how to interpret failing assert data so that failing asserts can be printed out in an understandable way. Shader Test Framework provides byte readers and byte writers to help users write tests that will produce well formatted error messages when asserts fail. Byte Readers are implemented in C++ and ByteWriters are implemented in HLSL.

**Contents**

1. [Byte Readers](#byte-readers)<br>
  a. [When do I need a Byte Reader](#when-do-i-need-a-byte-read)<br>
  b. [The Fundamental Byte Reader](#the-fundamental-byte-reader)<br>
  c. [The Default Byte Reader](#the-default-byte-reader)<br>
  d. [Creating a Byte Reader](#creating-a-byte-reader)<br>
  e. [Creating a Multi Type Byte Reader](#creating-a-multi-type-byte-reader)
2. [Byte Writers](#byte-writers)<br>
  a. [When do I need a Byte Writer](#when-do-i-need-a-byte-writer)<br>
  b. [Creating a Byte Writer](#creating-a-byte-writer)

## Byte Readers

A Byte reader is a function-like object that can be called with the following signature:
```c++
std::string(u16 InTypeId, std::span<const std::byte> InData)
```

`InData` is the span of bytes that represents the value that the byte reader will interpret and return as a `std::string`.
`InTypeId` is a 16 bit user provided value that is sent from HLSL to your Byte Reader based on the type of data. This can be used to support reading the bytes of multiple types with a single byte reader.

### When do I need a Byte Reader

A Byte Reader should be implemented when test writers want to customize how their HLSL types are printed in failure messages. Shader Test Framework provides two Byte Readers:

1. Default Byte Reader -> This will be used to print out the assert data of any types that a ByteReader has not been registered for. This Byte Reader will literally just print out the byte values of the data involved in the failed assert.
2. Fundamental Type Byte Reader -> This byte reader will be used for asserts that use any fundamental type in HLSL that is not a resource or an array. e.g. `float3`, `uint4x4`, `bool2` etc etc.

As there is a default byte reader that will catch all types, there is never any requirement to write your own byte readers. They are simply there if you wish to provide more useful error messages for when tests fail.

### The Fundamental Byte Reader

As mentioned, Shader Test Framework provides a byte reader that covers all fundamental types. This ensures that any test writer can get well formatted assert messages provided that they are asserting on primitive types. You can see them in action by uncommenting the function call in [FailingFundamentalTypeTests.hlsl](../../examples/Ex6_ByteReadersAndWriters/ShaderCode/FailingFundamentalTypeTests.hlsl) and then running the [ByteReadersAndWriters](../../examples/Ex6_ByteReadersAndWriters) example. Doing that should result in a failing test run with the following output:

```
ByteReadersAndWriters.cpp line 24: Failed
  REQUIRE( fixture.RunTest("Tests", 1, 1, 1) )
with expansion:
  REQUIRE( There were 0 successful asserts and 3 failed assertions
Assert 0: Line: 9 
Data 1: 42
Data 2: 24
Assert 1: Line: 10 
Data 1: 2, 2, 2
Data 2: 2, 2, 2
Assert 2: Line: 11 
Data 1: 
true, true
false, false
Data 2: 
false, false
true, true )
```

Let's go through this assert by assert:

1. Assert 0:

```c++
ASSERT(AreEqual, uint16_t(42), uint16_t(24));
```

This demonstrates that the byte reader can handle 16 bit types as well as the standard 32 bit types

2. Assert 1:

```c++
ASSERT(NotEqual, float64_t3(2.0, 2.0, 2.0), float64_t3(2.0, 2.0, 2.0));
```

This demonstrates both that vectors and 64 bit values are supported.

3. Assert 2

```c++
ASSERT(AreEqual, bool2x2(true, true, false, false), bool2x2(false, false, true, true));
```

This demonstrates that both matrices and boolean values are properly formatted.

### The Default Byte Reader

The default Byte Reader will handle formatting asserts for any type that does not have an associated Byte Reader. To see an example of the default byte reader in action, uncomment the following line in [FailingAssertWithoutByteReader.hlsl](../../examples/Ex6_ByteReadersAndWriters/ShaderCode/FailingAssertWithoutByteReader.hlsl).
```c++
//ASSERT(AreEqual, test1, test2);
```

Then run `Example6Tests - Failing test with no Byte Reader` in the [Ex6_ByteReadersAndWriters](../../examples/Ex6_ByteReadersAndWriters/) example project. Doing this should yield the following error message:

```
There were 0 successful asserts and 1 failed assertions
  Assert 0: Line: 26
  Data 1: Undefined Type -> Bytes: 0x0, 0x4000, 0x0
  Data 2: Undefined Type -> Bytes: 0x2a, 0x0, 0x0
```

Since the default byte reader has no type information to go on, it simply outputs the raw bytes of the objects captured in the assert. Often this is completely fine because an assert has to be failing to make use of the error message. Additionally, it is sometimes very little work to just take the byte representation and know what the values of your object were.

### Creating a Byte Reader

In the event that test writers wish to provide better error reporting than what the default byte reader can give, test writers are able to create their own byte readers. [FailingAssertWithByteReader.hlsl](../../examples/Ex6_ByteReadersAndWriters/ShaderCode/FailingAssertWithByteReader.hlsl) is a modification of the previous example that has had a byte reader implemented. Like in the previous example uncomment this line:

```c++
//ASSERT(AreEqual, test1, test2);
```

Then run `Example6Tests - Failing test with Byte Reader` in the [Ex6_ByteReadersAndWriters](../../examples/Ex6_ByteReadersAndWriters/) example project. Doing this should yield the following error message:

```
There were 0 successful asserts and 1 failed assertions
  Assert 0: Line: 32
  Data 1: a = 0, b = 2, c = false
  Data 2: a = 42, b = 0, c = false
```

As you can see, the error has been formatted using the information about the type. There two steps for making use of a Byte Reader in a test.

#### Step 1: Registering a Byte Reader with the fixture

The above example's byte reader is registered in the following lines of [ByteReadersAndWriters.cpp](../../examples/Ex6_ByteReadersAndWriters/ByteReadersAndWriters.cpp):

```c++
fixture.RegisterByteReader("MY_TYPE_READER_ID",
    [](const std::span<const std::byte> InData)
    {
        struct MyType
        {
            u32 a;
            f64 b;
            u32 c;
        };

        MyType val;
        std::memcpy(&val, InData.data(), sizeof(MyType));

        return std::format("a = {}, b = {}, c = {}", val.a, val.b, val.c ? "true" : "false");
    });
```

`RegisterByteReader` takes two parameters:

1. Name - This is the name of the Byte Reader. We will use this to make the HLSL tests aware of this byte reader in the next step. The framework passes this name to the HLSL test using a define. This is why the name given here is in the style of a C++ macro.
2. A lambda - This lambda takes a `const std::span<const std::byte>` and returns a `std::string`. The input parameter contains all of the bytes written as part of the failed assertion in HLSL. The output string is the string representation of the bytes that will be formatted into the failed assertion output.

In this example we simply create a struct that mimics the struct used in the HLSL tests. This makes it easy to just `memcpy` the bytes into an object of this type. You will notice that we are using a `u32` in place of a `bool` here. That is because in HLSL, `bool`s are 32 bit, whereas in C++ they are 8 bit. So we just use a 32 bit integer here and then interpret it while formatting the string.

#### Step 2: Making the HLSL test aware of the Byte Reader

Test writers need to be able to tell the framework what HLSL types use what Byte Reader on the C++ side. They do this by specializing `STF::ByteReaderTraits` in their HLSL tests. A specialization MUST provide the following member:

`static const uint16_t ReaderId`

The value of this should be set to the value of the name of the reader that was used when calling `RegisterByteReader`. The shader test framework provides a convenience base struct to make this easier. You can see this in [FailingAssertWithByteReader.hlsl](../../examples/Ex6_ByteReadersAndWriters/ShaderCode/FailingAssertWithByteReader.hlsl).

```c++
namespace STF
{
    template<>
    struct ByteReaderTraits<MyType> : ByteReaderTraitsBase<MY_TYPE_READER_ID>{};
}
```

Here we are providing the specialization of `STF::ByteReaderTraits` for `MyType` and we are using `STF::ByteReaderTraitsBase` for convenience.

And that is all that is required to register and use a byte reader.

### Creating a Multi Type Byte Reader

There can be cases where you have a family of types that can be formatted in a very similar way. If we were to register a byte reader for each type in this type family, we might have a huge amount of redundant code. In these cases, it can often be the case that a single byte reader could be used. In fact this is the case for the byte reader than handles fundamental types. It is one byte reader that can handle every scalar, vector and matrix fundamental type. Situations where Multi Type Byte Readers are useful:

1. Providing formatting for a templated type that has a number of specializations
2. Arrays of a type

In this example we will be demonstrating the second of the situations above. The HLSL for this example can be found in [FailingAssertWithMultiTypeByteReader.hlsl](../../examples/Ex6_ByteReadersAndWriters/ShaderCode/FailingAssertWithMultiTypeByteReader.hlsl). The C++ is in the `Example6Tests - Failing test with MultiType Byte Reader` scenario in [ByteReadersAndWriters.cpp](../../examples/Ex6_ByteReadersAndWriters/ByteReadersAndWriters.cpp)

If we uncomment the assert from this example and run the example we will get the following output:

```
There were 0 successful asserts and 1 failed assertions
  Assert 0: Line: 36
  Data 1: 0, 0, 2, 0
  Data 2: 0, 0, 0, 42
```

#### Multi Type Byte Readers in HLSL

If we are wanting to make use of a Multi Type Byte Reader in HLSL then there is a slight difference in how we specialize the `STF::ByteReaderTraits` template. Below is the specialization from the example:

```c++
template<uint InNum>
struct ByteReaderTraits<MyType<InNum> > : ByteReaderTraitsBase<MY_TYPE_READER_ID, InNum>{};
```

The difference here is that we are passing two values to the `STF::ByteReaderTraitsBase` template. We are passing the ReaderId like before. But we are also passing in the value `InNum` which in this example represents the number of elements in the array. This is because for Multi Type Byte Readers `STF::ByteReaderTraits` must have two members:

1. `static const uint16_t ReaderId` - Like with the single type byte reader, this is the id of the reader which is the value of the name of the reader that you use to register the byte reader with `RegisterByteReader`
2. `static const uint16_t TypeId` - This is an additional 16 bit integer that will be passed to the byte reader in C++. The value of this is test writer defined. It can be anything. In this case we are using the TypeId to pass through the size of the array. But it could be a value that represents a particular type. Or anything else.

#### Multi Type Byte Readers in C++

The only difference between single type and multi type byte readers in C++ is that multi type byte readers take an extra parameter:

```c++
fixture.RegisterByteReader("MY_TYPE_READER_ID",
    [](const u16 InTypeId, const std::span<const std::byte> InData)
    {
        auto val = std::make_unique_for_overwrite<u32[]>(InTypeId);
        std::memcpy(val.get(), InData.data(), InData.size_bytes());

        std::stringstream ret;
        ret << val[0];

        for (u16 index = 1; index < InTypeId; ++index)
        {
            ret << ", " << val[index];
        }

        return ret.str();
    });
```

As mentioned above, we are using this extra parameter to pass the size of the array through.

## Byte Writers

Fundamentally, a ByteWriter is a struct template specialization of `ttl::byte_writer` of a particular type, `T`. It is only an HLSL construct. 

### When do I need a Byte Writer?

ByteWriters are likely not something that will be commonly written by test writers. This is due to [ByteAddressBuffer Load Store Additions](https://github.com/microsoft/DirectXShaderCompiler/wiki/ByteAddressBuffer-Load-Store-Additions). If doing a `memcpy` of the bytes of objects of your type, `T`, is enough to copy your object, then no ByteWriter is necessary. Shader Test Framework will just do the `memcpy` for you.

Where Byte Writers come in handy is when your type refers to other memory. e.g. a `span` like object.

### Creating a Byte Writer

To create a Byte Writer of a type, `T`, then specialize `ttl::byte_writer` for your type. Specializations are required to implement the following functions:

1. `static uint bytes_required(T)` -> This function will take an object of type, `T`, and return the number of bytes that that particular object will require to store its value.
2. `static uint alignment_required(T)` -> This function will report the necessary alignment.
3. `template<typename U> static void write(inout ttl::container_wrapper<U> ContainerToWriteTo, uint IndexToStartWritingAt, T ObjectToWrite)` -> This function will do the actual writing of the data to a container wrapped in a [`ttl::container_wrapper`](../TTL/ContainerWrapper.md) NOTE: It is very important that `ContainerToWriteTo` is marked as `inout` to ensure that it is returned from the function.

They are also required to have the following member:
```c++
static const bool has_writer = true;
```

#### Byte Writers in HLSL 

You can find an example of this in [FailingAssertWithByteWriter.hlsl](../../examples/Ex6_ByteReadersAndWriters/ShaderCode/FailingAssertWithByteWriter.hlsl). This example is run from the `Example6Tests - Failing test with Byte Writer` scenario in [ByteReadersAndWriters.cpp](../../examples/Ex6_ByteReadersAndWriters/ByteReadersAndWriters.cpp). Before running the example make sure to uncomment the ASSERT so that the test actually fails and you see the failed assert info.

This example has the following type:

```c++
struct MyIntsSpan
{
    uint Start;
    uint End;

    // Other Functions
};
```

This type represents a span of some elements in another array. In this example this array is:

```c++
const static int MyInts[] = {2, 4, 5, 6, 7, 42, 512, 1024, 3};
```

If an assert were to fail that involves the `MyIntsSpan` then the bytes written to the assert buffer would just be the `Start` and `End` values. This might be fine. However, it could be better if the bytes that are printed out would be the values that the span represents in the array. To achieve this we can create a `ttl::byte_writer` specialization.

The relevant code from this example is the following:

```
namespace ttl
{
    template<>
    struct byte_writer<MyIntsSpan>
    {
        static const bool has_writer = true;

        static uint bytes_required(MyIntsSpan In)
        {
            return (In.End - In.Start) * ttl::size_of<int>::value;
        }

        static uint alignment_required(MyIntsSpan In)
        {
            return ttl::align_of<int>::value;
        }

        // inout here is very important to make sure that the Container is returned out of the function
        template<typename U>
        static void write(inout container_wrapper<U> InContainer, const uint InIndex, const MyIntsSpan In)
        {
            const uint size = In.End - In.Start;
            for (uint i = 0; i < size; ++i)
            {
                InContainer.store(InIndex + i * 4u, MyInts[In.Start + i]);
            }
        }
    };
}
```

In this example, we have implemented the 3 necessary functions and provided the `has_writer` member.

When we run this example we will get the following output:

```
There were 0 successful asserts and 1 failed assertions
  Assert 0: Line: 69
  Data 1: 4, 5, 6
  Data 2: 5, 6, 7, 42, 512
```

As you can see our failed asserted data is what we wrote in the `write` function of the `ttl::byte_writer` specialization rather than the the simple `Start` and `End` indices.


---

[Top](#byte-readers-and-writers)
