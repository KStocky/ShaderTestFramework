# Byte Readers and Writers

The HLSL and C++ sides of the framework need to be able to communicate how to interpret failing assert data so that failing asserts can be printed out in an understandable way. Shader Test Framework provides byte readers and byte writers to help users write tests that will produce well formatted error messages when asserts fail. Byte Readers are implemented in C++ and ByteWriters are implemented in HLSL.

## Byte Readers

A Byte reader is a function-like object that can be called with the following signature:
```c++
void(u16 InTypeId, std::span<const std::byte> InData)
```

### When do I need a Byte Reader

A Byte Reader should be implemented when test writers want to customize how their HLSL types are printed in failure messages. Shader Test Framework provides two Byte Readers:

1. Default Byte Reader -> This will be used to print out the assert data of any types that a ByteReader has not been registered for.
2. Fundamental Type Byte Reader -> This byte reader will be used for asserts that use any fundamental type in HLSL that is not a resource. e.g. `float3`, `uint4x4`, `bool2` etc etc.

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
