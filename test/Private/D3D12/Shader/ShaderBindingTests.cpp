
#include <Platform.h>
#include <D3D12/Shader/ShaderBinding.h>
#include <Framework/HLSLTypes.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

namespace ShaderBindingConstructionTests
{
    template<typename T>
    concept TestConstruction = requires(T In)
    {
        { stf::ShaderBinding{"Test", In} };
    };

    struct TriviallyCopyable
    {
        int Val;
    };

    struct NotTriviallyCopyable
    {
        NotTriviallyCopyable(const NotTriviallyCopyable&) {}
        NotTriviallyCopyable& operator=(const NotTriviallyCopyable&) { return *this; }
    };

    static_assert(TestConstruction<TriviallyCopyable>, "Expected a shader binding to be valid for this type");
    static_assert(!TestConstruction<NotTriviallyCopyable>, "Expected a shader binding to not be valid for this type");
}

namespace ShaderBindingTestsPrivate
{
    template<typename T>
    T Cast(const std::span<const std::byte> InData)
    {
        T ret;
        std::memcpy(&ret, InData.data(), sizeof(T));
        return ret;
    }
}

namespace ShaderBindingTemplateTests
{
    using namespace stf;
    TEMPLATE_TEST_CASE_SIG("Templated ShaderBindingTests", "[template][nttp]", ((typename T, T Val), T, Val),
        (i32, 0), (i32, 123456789),
        (float, 0.0f), (float, 42.5f),
        (float4, float4{0.0f, 1.0, 100.0, 1000.0f})
    )
    {
        using namespace ShaderBindingTestsPrivate;

        const std::string name = "Test";
        const T data = Val;
        ShaderBinding test(name, data);
        const T actualData = Cast<T>(test.GetBindingData());

        REQUIRE(test.GetName() == name);
        REQUIRE(data == actualData);
    }
}
