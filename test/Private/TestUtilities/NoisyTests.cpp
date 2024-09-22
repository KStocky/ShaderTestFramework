
#include "TestUtilities/Noisy.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("Noisy - DefaultNoisyLogger")
{
    DefaultNoisyLogger::LogData = DefaultLogData{};

    GIVEN("Newly constructed Log Data")
    {
        THEN("All data is zeroed")
        {
            REQUIRE(DefaultNoisyLogger::LogData == DefaultLogData{});
        }
    }

    GIVEN("A Noisy has been constructed")
    {
        Noisy noisy;

        THEN("Num constructions is 1")
        {
            REQUIRE(DefaultNoisyLogger::LogData ==
                DefaultLogData
                {
                    .NumConstructions = 1
                });
        }

        WHEN("Destructed")
        {
            noisy.~Noisy();

            THEN("Num Destructions is 1")
            {
                REQUIRE(DefaultNoisyLogger::LogData ==
                    DefaultLogData
                    {
                        .NumConstructions = 1,
                        .NumDestructions = 1
                    });
            }
        }

        WHEN("Copy constructed")
        {
            Noisy noisy2(noisy);

            THEN("NumCopyConstructions is 1")
            {
                REQUIRE(DefaultNoisyLogger::LogData ==
                    DefaultLogData
                    {
                        .NumConstructions = 1,
                        .NumCopyConstructions = 1
                    });
            }
        }

        WHEN("Move constructed")
        {
            Noisy noisy2(std::move(noisy));

            THEN("NumMoveConstructions is 1")
            {
                REQUIRE(DefaultNoisyLogger::LogData ==
                    DefaultLogData
                    {
                        .NumConstructions = 1,
                        .NumMoveConstructions = 1
                    });
            }
        }

        WHEN("Event called")
        {
            static constexpr stf::i32 expectedEventId = 42;
            noisy.Event(expectedEventId);

            THEN("Expected event with that id to have a value of 1")
            {
                REQUIRE(DefaultNoisyLogger::LogData ==
                    DefaultLogData
                    {
                        .NumEvents
                        {
                            {expectedEventId, 1}
                        },
                        .NumConstructions = 1
                    });
            }
        }
    }

    GIVEN("Two noisy objects")
    {
        Noisy noisy1;
        Noisy noisy2;

        THEN("Num constructions is 2")
        {
            REQUIRE(DefaultNoisyLogger::LogData ==
                DefaultLogData
                {
                    .NumConstructions = 2
                });
        }

        WHEN("First is destroyed")
        {
            noisy1.~Noisy();

            THEN("Num Destructions is 1")
            {
                REQUIRE(DefaultNoisyLogger::LogData ==
                    DefaultLogData
                    {
                        .NumConstructions = 2,
                        .NumDestructions = 1
                    });
            }

            AND_WHEN("Second is destroyed")
            {
                noisy2.~Noisy();

                THEN("Num Destructions is 2")
                {
                    REQUIRE(DefaultNoisyLogger::LogData ==
                        DefaultLogData
                        {
                            .NumConstructions = 2,
                            .NumDestructions = 2
                        });
                }
            }
        }

        WHEN("Copy assigned")
        {
            noisy1 = noisy2;

            THEN("Num Copy Assignments is 1")
            {
                REQUIRE(DefaultNoisyLogger::LogData ==
                    DefaultLogData
                    {
                        .NumConstructions = 2,
                        .NumCopyAssignments = 1
                    });
            }
        }

        WHEN("Event called on first")
        {
            static constexpr stf::i32 expectedFirstEventId = 42;

            noisy1.Event(expectedFirstEventId);

            THEN("Expected first event to have 1 call")
            {
                REQUIRE(DefaultNoisyLogger::LogData ==
                    DefaultLogData
                    {
                        .NumEvents
                        {
                            {expectedFirstEventId, 1}
                        },
                        .NumConstructions = 2
                    });
            }

            AND_WHEN("First event id called again on second noisy")
            {
                noisy2.Event(expectedFirstEventId);

                THEN("Expected first event to have 2 calls")
                {
                    REQUIRE(DefaultNoisyLogger::LogData ==
                        DefaultLogData
                        {
                            .NumEvents
                            {
                                {expectedFirstEventId, 2}
                            },
                            .NumConstructions = 2
                        });
                }
            }

            AND_WHEN("Second event id called on second noisy")
            {
                static constexpr stf::i32 expectedSecondEventId = 2;
                noisy2.Event(expectedSecondEventId);

                THEN("Expected first event to have 2 calls")
                {
                    REQUIRE(DefaultNoisyLogger::LogData ==
                        DefaultLogData
                        {
                            .NumEvents
                            {
                                {expectedFirstEventId, 1},
                                {expectedSecondEventId, 1}
                            },
                            .NumConstructions = 2
                        });
                }
            }
        }

        WHEN("Move assigned")
        {
            noisy1 = std::move(noisy2);

            THEN("Num Move Assignments is 1")
            {
                REQUIRE(DefaultNoisyLogger::LogData ==
                    DefaultLogData
                    {
                        .NumConstructions = 2,
                        .NumMoveAssignments = 1
                    });
            }
        }
    }
}