include_guard(GLOBAL)
include(FetchContent)

function(add_catch2 IN_TARGET)
    FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.11.0
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
        tuplet
        GIT_REPOSITORY https://github.com/KStocky/tuplet
        GIT_TAG MyMain
    )

    FetchContent_MakeAvailable(Tuplet)
    
    file(GLOB_RECURSE TUPLET_HEADERS "${tuplet_SOURCE_DIR}/include/*.h*" )
    target_link_libraries(${IN_TARGET} PUBLIC tuplet::tuplet)
    target_sources(${IN_TARGET} PRIVATE ${TUPLET_HEADERS})
    source_group(TREE ${tuplet_SOURCE_DIR}/include/tuplet PREFIX "ThirdParty/tuplet" FILES ${TUPLET_HEADERS})

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
    source_group(TREE ${float16_fetch_SOURCE_DIR}/include/float16 PREFIX "ThirdParty/float16" FILES ${FLOAT16_HEADERS})
    return()
endfunction()