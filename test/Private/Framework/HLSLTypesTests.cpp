#include "Framework/HLSLTypes.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include <random>

namespace HLSLTypeTests
{
    using namespace stf;

    template<typename T, u32 NumComponents>
    struct HLSLTypeOpTestFixture
    {

        using Type = HLSLVector<T, NumComponents>;

        template<typename U>
        struct DistTraits;

        template<>
        struct DistTraits<float>
        {
            using Type = std::uniform_real_distribution<float>;
            static constexpr float Min = -100.0f;
            static constexpr float Max = 100.0f;
        };

        template<>
        struct DistTraits<f16> : DistTraits<float> {};

        template<>
        struct DistTraits<f64> : DistTraits<float> {};

        template<>
        struct DistTraits<u32>
        {
            using Type = std::uniform_int_distribution<u32>;
            static constexpr u32 Min = 0;
            static constexpr u32 Max = 200;
        };

        template<>
        struct DistTraits<u16> : DistTraits<u32> {};

        template<>
        struct DistTraits<u64> : DistTraits<u32> {};

        template<>
        struct DistTraits<i32>
        {
            using Type = std::uniform_int_distribution<i32>;
            static constexpr i32 Min = -100;
            static constexpr i32 Max = 100;
        };

        template<>
        struct DistTraits<i16> : DistTraits<i32> {};

        template<>
        struct DistTraits<i64> : DistTraits<i32> {};

        enum class EOp
        {
            Add,
            Subtract,
            Multiply,
            Divide,
            NumOps
        };

        class VectorOpTest
        {
        public:

            VectorOpTest() = default;
            VectorOpTest(const Type InFirst, const Type InSecond)
                : m_First(InFirst)
                , m_Second(InSecond)
            {}

            Type Op(const EOp InOp) const
            {
                switch (InOp)
                {
                case EOp::Add:
                    return m_First + m_Second;

                case EOp::Divide:
                    return m_First / m_Second;

                case EOp::Multiply:
                    return m_First * m_Second;

                case EOp::Subtract:
                    return m_First - m_Second;

                default:
                    return Type{};
                }
            }

            T ExpectedOp(const T InFirst, const T InSecond, const EOp InOp) const
            {
                switch (InOp)
                {
                case EOp::Add:
                    return InFirst + InSecond;

                case EOp::Divide:
                    return InFirst / InSecond;

                case EOp::Multiply:
                    return InFirst * InSecond;

                case EOp::Subtract:
                    return InFirst - InSecond;

                default:
                    return T{};
                }
            }

            bool Test(const EOp InOp) const
            {
                const auto actual = Op(InOp);

                if constexpr (NumComponents == 2)
                {
                    return
                        actual.x == ExpectedOp(m_First.x, m_Second.x, InOp) &&
                        actual.y == ExpectedOp(m_First.y, m_Second.y, InOp);
                }
                else if constexpr (NumComponents == 3)
                {
                    return
                        actual.x == ExpectedOp(m_First.x, m_Second.x, InOp) &&
                        actual.y == ExpectedOp(m_First.y, m_Second.y, InOp) &&
                        actual.z == ExpectedOp(m_First.z, m_Second.z, InOp);
                }
                else
                {
                    return
                        actual.x == ExpectedOp(m_First.x, m_Second.x, InOp) &&
                        actual.y == ExpectedOp(m_First.y, m_Second.y, InOp) &&
                        actual.z == ExpectedOp(m_First.z, m_Second.z, InOp) &&
                        actual.w == ExpectedOp(m_First.w, m_Second.w, InOp);
                }
            }

            Type First() const { return m_First; }
            Type Second() const { return m_Second; }

        private:
            Type m_First{};
            Type m_Second{};
        };

        static constexpr size_t NumOPs = 20;

        HLSLTypeOpTestFixture()
        {
            m_Ops.reserve(NumOPs);
            std::random_device rd;
            std::mt19937 gen(rd());
            typename DistTraits<T>::Type dis(DistTraits<T>::Min, DistTraits<T>::Max);

            auto GenNum = [&]() -> T
                {
                    T num = static_cast<T>(dis(gen));
                    if (num == static_cast<T>(0))
                    {
                        return static_cast<T>(1);
                    }

                    return num;
                };

            for (u32 opIndex = 0; opIndex < NumOPs; ++opIndex)
            {
                if constexpr (NumComponents == 2)
                {
                    m_Ops.emplace_back(Type(GenNum(), GenNum()), Type(GenNum(), GenNum()));
                }
                else if constexpr (NumComponents == 3)
                {
                    m_Ops.emplace_back(Type(GenNum(), GenNum(), GenNum()), Type(GenNum(), GenNum(), GenNum()));
                }
                else
                {
                    m_Ops.emplace_back(Type(GenNum(), GenNum(), GenNum(), GenNum()), Type(GenNum(), GenNum(), GenNum(), GenNum()));
                }
            }
        }

        operator bool() const
        {
            for (const auto& test : m_Ops)
            {
                for (u32 opIndex = 0; opIndex < static_cast<u32>(EOp::NumOps); ++opIndex)
                {
                    if (!test.Test(static_cast<EOp>(opIndex)))
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        std::vector<VectorOpTest> m_Ops;
    };

    TEMPLATE_TEST_CASE_SIG("HLSL Type operation Tests", "[template][nttp]", ((typename T, u32 S), T, S),
        (i16, 2), (i16, 3), (i16, 4),
        (i32, 2), (i32, 3), (i32, 4),
        (i64, 2), (i64, 3), (i64, 4),
        (u16, 2), (u16, 3), (u16, 4),
        (u32, 2), (u32, 3), (u32, 4),
        (u64, 2), (u64, 3), (u64, 4),
        (f16, 2), (f16, 3), (f16, 4),
        (f32, 2), (f32, 3), (f32, 4),
        (f64, 2), (f64, 3), (f64, 4)
    )
    {
        HLSLTypeOpTestFixture<T, S> fixture;
        REQUIRE(fixture);
    }

    namespace HLSLTypesConceptsTests
    {
        static_assert(HLSLTypeTriviallyConvertibleType<bool2>);
        static_assert(HLSLTypeTriviallyConvertibleType<bool3>);
        static_assert(HLSLTypeTriviallyConvertibleType<bool4>);
        static_assert(HLSLTypeTriviallyConvertibleType<int16_t2>);
        static_assert(HLSLTypeTriviallyConvertibleType<int16_t3>);
        static_assert(HLSLTypeTriviallyConvertibleType<int16_t4>);
        static_assert(HLSLTypeTriviallyConvertibleType<int2>);
        static_assert(HLSLTypeTriviallyConvertibleType<int3>);
        static_assert(HLSLTypeTriviallyConvertibleType<int4>);
        static_assert(HLSLTypeTriviallyConvertibleType<int64_t2>);
        static_assert(HLSLTypeTriviallyConvertibleType<int64_t3>);
        static_assert(HLSLTypeTriviallyConvertibleType<int64_t4>);
        static_assert(HLSLTypeTriviallyConvertibleType<uint16_t2>);
        static_assert(HLSLTypeTriviallyConvertibleType<uint16_t3>);
        static_assert(HLSLTypeTriviallyConvertibleType<uint16_t4>);
        static_assert(HLSLTypeTriviallyConvertibleType<uint2>);
        static_assert(HLSLTypeTriviallyConvertibleType<uint3>);
        static_assert(HLSLTypeTriviallyConvertibleType<uint4>);
        static_assert(HLSLTypeTriviallyConvertibleType<uint64_t2>);
        static_assert(HLSLTypeTriviallyConvertibleType<uint64_t3>);
        static_assert(HLSLTypeTriviallyConvertibleType<uint64_t4>);
        static_assert(HLSLTypeTriviallyConvertibleType<float16_t2>);
        static_assert(HLSLTypeTriviallyConvertibleType<float16_t3>);
        static_assert(HLSLTypeTriviallyConvertibleType<float16_t4>);
        static_assert(HLSLTypeTriviallyConvertibleType<float2>);
        static_assert(HLSLTypeTriviallyConvertibleType<float3>);
        static_assert(HLSLTypeTriviallyConvertibleType<float4>);
        static_assert(HLSLTypeTriviallyConvertibleType<float64_t2>);
        static_assert(HLSLTypeTriviallyConvertibleType<float64_t3>);
        static_assert(HLSLTypeTriviallyConvertibleType<float64_t4>);
    }
}
