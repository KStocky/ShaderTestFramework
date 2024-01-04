include_guard(GLOBAL)
include(AssetDependencyManagement)
include(ExternalProjects)

function(add_stf_with_catch2 IN_TARGET)

    target_compile_definitions(${IN_TARGET} PRIVATE USE_PIX)
    target_link_libraries(${IN_TARGET} PRIVATE ShaderTestFramework)

    copy_all_dependent_assets(${IN_TARGET})
    add_catch2(${IN_TARGET})
    return()
endfunction()