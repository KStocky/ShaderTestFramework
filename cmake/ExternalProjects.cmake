include_guard(GLOBAL)
include(FetchContent)

function(add_catch2 IN_TARGET)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
    )

    FetchContent_MakeAvailable(Catch2)
    
    target_link_libraries(${IN_TARGET} PRIVATE Catch2::Catch2WithMain)

    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

    include(Catch)
    catch_discover_tests(${IN_TARGET})
    return()
endfunction()

function(add_tl_expected IN_TARGET)

    set(EXPECTED_DIR ${CMAKE_HOME_DIRECTORY}/extern/tl-expected/)
    file(DOWNLOAD
        https://raw.githubusercontent.com/TartanLlama/expected/master/include/tl/expected.hpp
        ${EXPECTED_DIR}/include/tl/expected.hpp
    )

    target_include_directories(${IN_TARGET} PUBLIC ${EXPECTED_DIR}/include)
    return()
endfunction()

function(add_tuplet IN_TARGET)
    FetchContent_Declare(
        Tuplet
        GIT_REPOSITORY https://github.com/codeinred/tuplet.git
        GIT_TAG v2.1.1
    )

    FetchContent_MakeAvailable(Tuplet)
    
    target_link_libraries(${IN_TARGET} PUBLIC tuplet)
    return()
endfunction()