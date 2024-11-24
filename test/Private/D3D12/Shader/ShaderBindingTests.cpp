
#include <Platform.h>
#include <D3D12/Shader/ShaderBinding.h>

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

TEST_CASE("ShaderBindingTests")
{
    using namespace stf;

    auto Cast = []<typename T>(const std::span<const std::byte> InData) -> T
    {
        T ret;
        std::memcpy(&ret, InData.data(), sizeof(T));
        return ret;
    };

    SECTION("An int initialized to zero")
    {
        const std::string name = "Test";
        const i32 data = 0;
        ShaderBinding test(name, data);
        const auto actualData = Cast.operator()<i32>(test.GetBindingData());

        REQUIRE(test.GetName() == name);
        REQUIRE(data == actualData);
    }

    SECTION("An int initialized to non zero")
    {
        const std::string name = "Test";
        const i32 data = 123456789;
        ShaderBinding test(name, data);
        const auto actualData = Cast.operator()<i32>(test.GetBindingData());

        REQUIRE(test.GetName() == name);
        REQUIRE(data == actualData);
    }
}
