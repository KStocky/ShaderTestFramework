
#include "TestUtilities/Noisy.h"
#include <Stats/ScopedObject.h>
#include <Utility/Concepts.h>

#include <catch2/catch_test_macros.hpp>

namespace ScopedObjectCompileTests
{
    using namespace stf;
    using CallableType = decltype([]() {});
    
    struct NonCallableType {};

    static_assert(InstantiatableFrom<ScopedObject, CallableType>);
    static_assert(!InstantiatableFrom<ScopedObject, NonCallableType>);
    static_assert(!DefaultConstructibleType<ScopedObject<CallableType>>);

    static_assert(!std::constructible_from<ScopedObject<CallableType>, CallableType>);
    static_assert(!std::constructible_from<ScopedObject<CallableType>, NonCallableType>);

    static_assert(!std::copyable<ScopedObject<CallableType>>);
    static_assert(!std::movable<ScopedObject<CallableType>>);

    static_assert(std::constructible_from<ScopedObject<CallableType>, CallableType, CallableType>);

    static_assert(!Newable<ScopedObject<CallableType>, CallableType, CallableType>);
}

SCENARIO("ScopedObjectTests")
{
    using namespace stf;
    GIVEN("Noisy Callables")
    {
        class NoisyCallable
        {
        public:
            NoisyCallable(const i32 InId)
                : m_Id(InId)
            {}

            void operator()() const
            {
                m_Noisy.Event(m_Id);
            }

        private:
            i32 m_Id = 0;
            DefaultNoisy m_Noisy;
        };

        static constexpr i32 onConstructionId = 24;
        static constexpr i32 onDestructionId = 42;

        DefaultNoisyLogger::LogData = DefaultLogData{};

        WHEN("Scoped Object is constructed with unnamed callables")
        {
            {
                ScopedObject test{ NoisyCallable { onConstructionId } , NoisyCallable { onDestructionId } };

                THEN("Expected calls are made")
                {
                    REQUIRE(DefaultNoisyLogger::LogData == DefaultLogData
                        {
                            .NumEvents =
                            {
                                {onConstructionId, 1}
                            },
                            .NumConstructions = 2,
                            .NumMoveConstructions = 1,
                            .NumDestructions = 2
                        });
                }
            }

            AND_WHEN("Scoped Object goes out of scope")
            {
                THEN("Expected calls are made")
                {
                    REQUIRE(DefaultNoisyLogger::LogData == DefaultLogData
                        {
                            .NumEvents =
                            {
                                {onConstructionId, 1},
                                {onDestructionId, 1}
                            },
                            .NumConstructions = 2,
                            .NumMoveConstructions = 1,
                            .NumDestructions = 3
                        });
                }
            }
        }

        WHEN("Scoped Object is constructed with named callables")
        {
            {
                NoisyCallable construct{ onConstructionId };
                NoisyCallable destruct{ onDestructionId };
                ScopedObject test{ construct, destruct };

                THEN("Expected calls are made")
                {
                    REQUIRE(DefaultNoisyLogger::LogData == DefaultLogData
                        {
                            .NumEvents =
                            {
                                {onConstructionId, 1}
                            },
                            .NumConstructions = 2,
                            .NumCopyConstructions = 1,
                        });
                }
            }

            AND_WHEN("Scoped Object goes out of scope")
            {
                THEN("Expected calls are made")
                {
                    REQUIRE(DefaultNoisyLogger::LogData == DefaultLogData
                        {
                            .NumEvents =
                            {
                                {onConstructionId, 1},
                                {onDestructionId, 1}
                            },
                            .NumConstructions = 2,
                            .NumCopyConstructions = 1,
                            .NumDestructions = 3
                        });
                }
            }
        }

        WHEN("Scoped Object is constructed with moved named callables")
        {
            {
                NoisyCallable construct{ onConstructionId };
                NoisyCallable destruct{ onDestructionId };
                ScopedObject test{ std::move(construct), std::move(destruct) };

                THEN("Expected calls are made")
                {
                    REQUIRE(DefaultNoisyLogger::LogData == DefaultLogData
                        {
                            .NumEvents =
                            {
                                {onConstructionId, 1}
                            },
                            .NumConstructions = 2,
                            .NumMoveConstructions = 1,
                        });
                }
            }

            AND_WHEN("Scoped Object goes out of scope")
            {
                THEN("Expected calls are made")
                {
                    REQUIRE(DefaultNoisyLogger::LogData == DefaultLogData
                        {
                            .NumEvents =
                            {
                                {onConstructionId, 1},
                                {onDestructionId, 1}
                            },
                            .NumConstructions = 2,
                            .NumMoveConstructions = 1,
                            .NumDestructions = 3
                        });
                }
            }
        }

    }
}
