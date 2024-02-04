include_guard(GLOBAL)
include(FetchContent)

function(add_catch2 IN_TARGET)
    FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
    )

    FetchContent_MakeAvailable(Catch2)
    
    target_link_libraries(${IN_TARGET} PRIVATE Catch2::Catch2WithMain)

    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

    include(Catch)
    catch_discover_tests(${IN_TARGET} WORKING_DIRECTORY "$<TARGET_FILE_DIR:${IN_TARGET}>")
    return()
endfunction()

function(add_tl_expected IN_TARGET)

    FetchContent_Declare(tl_expected
        GIT_REPOSITORY https://github.com/TartanLlama/expected.git
        GIT_TAG master
        )
    
    FetchContent_GetProperties(tl_expected)
    if (NOT tl_expected_POPULATED)
        FetchContent_Populate(tl_expected)
    endif()

    file(GLOB_RECURSE EXPECTED_HEADERS "${tl_expected_SOURCE_DIR}/include/*.h*" )
    target_include_directories(${IN_TARGET} PUBLIC ${tl_expected_SOURCE_DIR}/include)
    target_sources(${IN_TARGET} PRIVATE ${EXPECTED_HEADERS})
    source_group(TREE ${tl_expected_SOURCE_DIR}/include PREFIX "ThirdParty/tl-expected" FILES ${EXPECTED_HEADERS})
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
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        )
    
    FetchContent_MakeAvailable(float16_fetch)

    file(GLOB_RECURSE FLOAT16_HEADERS "${float16_fetch_SOURCE_DIR}/include/*.h*" )
    target_include_directories(${IN_TARGET} PUBLIC ${float16_fetch_SOURCE_DIR}/include)
    target_sources(${IN_TARGET} PRIVATE ${FLOAT16_HEADERS})
    source_group(TREE ${float16_fetch_SOURCE_DIR}/include PREFIX "ThirdParty/float16" FILES ${FLOAT16_HEADERS})
    return()
endfunction()