# Byte Readers and Writers

The HLSL and C++ sides of the framework need to be able to communicate how to interpret failing assert data so that failing asserts can be printed out in an understandable way. Shader Test Framework provides byte readers and byte writers to help users write tests that will produce well formatted error messages when asserts fail. Byte Readers are implemented in C++ and ByteWriters are implemented in HLSL.

**Contents**

1. [Byte Readers](#byte-readers)<br>
  a. [When do I need a Byte Reader](#when-do-i-need-a-byte-reader)<br>
  b. [The Fundamental Byte Reader](#the-fundamental-byte-reader)<br>
  c. [The Default Byte Reader](#the-default-byte-reader)
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

As mentioned, Shader Test Framework provides a byte reader that covers all fundamental types. This ensures that any test writer can get well formatted assert messages provided that they are asserting on primitive types. You can see them in action by uncommenting the function call in [FailingFundamentalTypeTests.hlsl](../examples/Ex6_ByteReadersAndWriters/ShaderCode/FailingFundamentalTypeTests.hlsl) and then running the [ByteReadersAndWriters](../examples/Ex6_ByteReadersAndWriters) example. Doing that should result in a failing test run with the following output:

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

The default Byte Reader will handle formatting asserts for any type that does not have an associated Byte Reader. To see an example of the default byte reader in action, uncomment the following line in [FailingAssertWithoutByteReader.hlsl](../examples/Ex6_ByteReadersAndWriters/ShaderCode/FailingAssertWithoutByteReader.hlsl).
```c++
//ASSERT(AreEqual, test1, test2);
```

Then run `Example6Tests - Failing test with no Byte Reader` in the [Ex6_ByteReadersAndWriters](../examples/Ex6_ByteReadersAndWriters/) example project. Doint this should yield the following error message:

```
There were 0 successful asserts and 1 failed assertions
  Assert 0: Line: 26
  Data 1: Undefined Type -> Bytes: 0x0, 0x4000, 0x0
  Data 2: Undefined Type -> Bytes: 0x2a, 0x0, 0x0
```

Since the default byte reader has no type information to go on, it simple outputs the raw bytes of the objects captured in the assert. Often this is completely fine because you should an assert has to be failing to make use of the error message. Additionally, it is sometimes very little work to just take the byte representation and know what the values of your object were.

## Byte Writers

Fundamentally, a ByteWriter is a struct template specialization of `ttl::byte_writer` of a particular type, `T`. It is only an HLSL construct. 

### When do I need a Byte Writer?

ByteWriters are likely not something that will be commonly written by test writers. This is due to [ByteAddressBuffer Load Store Additions](https://github.com/microsoft/DirectXShaderCompiler/wiki/ByteAddressBuffer-Load-Store-Additions). If doing a `memcpy` of the bytes of objects of your type, `T`, is enough to copy your object, then no ByteWriter is necessary. Shader Test Framework will just do the `memcpy` for you.

Where Byte Writers come in handy is when your type refers to other memory. e.g. a `span` like object.

### Creating a Byte Writer

To create a Byte Writer of a type, `T`, then specialize `ttl::byte_writer` for your type. Specializations are required to implement the following functions:

1. `static uint bytes_required(T)` -> This function will take an object of type, `T`, and return the number of bytes that that particular object will require to store its value.
2. `static uint alignment_required(T)` -> This function will report the necessary alignment.
3. `template<typename U> static void write(inout ttl::container_wrapper<U> ContainerToWriteTo, uint IndexToStartWritingAt, T ObjectToWrite)` -> This function will do the actual writing of the data to a container wrapped in a [`ttl::container_wrapper`](./ContainerWrapper.md) NOTE: It is very important that `ContainerToWriteTo` is marked as `inout` to ensure that it is returned from the function.
