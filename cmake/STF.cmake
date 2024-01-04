include_guard(GLOBAL)
include(AssetDependencyManagement)
include(ExternalProjects)
include(FetchContent)

function(add_stf_with_catch2 IN_TARGET)

    asset_dependency_init(${IN_TARGET})

    FetchContent_Declare(
        stf
        GIT_REPOSITORY https://github.com/KStocky/ShaderTestFramework
        GIT_TAG FetchContentAndDocs
    )

    FetchContent_MakeAvailable(stf)

    list(APPEND CMAKE_MODULE_PATH ${stf_SOURCE_DIR}/cmake)

    target_compile_definitions(${IN_TARGET} PRIVATE USE_PIX)
    target_link_libraries(${IN_TARGET} PRIVATE ShaderTestFramework)

    copy_all_dependent_assets(${IN_TARGET})
    add_catch2(${IN_TARGET})
endfunction()