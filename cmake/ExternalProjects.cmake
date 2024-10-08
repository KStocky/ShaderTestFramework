include_guard(GLOBAL)
include(FetchContent)

function(add_catch2 IN_TARGET)
    FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG f7cd0ba0511b61592e70f07533d7ea30a8a5395d
    )

    FetchContent_MakeAvailable(Catch2)
    
    target_link_libraries(${IN_TARGET} PRIVATE Catch2::Catch2WithMain)

    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

    include(Catch)
    catch_discover_tests(${IN_TARGET}
        EXTRA_ARGS "--durations yes"
        WORKING_DIRECTORY "$<TARGET_FILE_DIR:${IN_TARGET}>"
        REPORTER junit
        OUTPUT_DIR Reports
        OUTPUT_SUFFIX ".xml"
        DISCOVERY_MODE PRE_TEST
        )
    return()
endfunction()

function(add_tuplet IN_TARGET)
    FetchContent_Declare(
        Tuplet
        GIT_REPOSITORY https://github.com/codeinred/tuplet.git
        GIT_TAG v2.1.1
    )

    FetchContent_MakeAvailable(Tuplet)
    
    target_link_libraries(${IN_TARGET} PUBLIC tuplet::tuplet)
    return()
endfunction()

function(add_float16 IN_TARGET)
    FetchContent_Declare(float16_fetch
        GIT_REPOSITORY https://github.com/KStocky/float16_t.git
        GIT_TAG master
        )
    
    FetchContent_MakeAvailable(float16_fetch)

    file(GLOB_RECURSE FLOAT16_HEADERS "${float16_fetch_SOURCE_DIR}/include/*.h*" )
    target_include_directories(${IN_TARGET} PUBLIC ${float16_fetch_SOURCE_DIR}/include)
    target_sources(${IN_TARGET} PRIVATE ${FLOAT16_HEADERS})
    source_group(TREE ${float16_fetch_SOURCE_DIR}/include PREFIX "ThirdParty/float16" FILES ${FLOAT16_HEADERS})
    return()
endfunction()