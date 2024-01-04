include(AssetDependencyManagement)
include(ExternalProjects)

set(TARGET_NAME Catch2Example)

add_executable(${TARGET_NAME})
asset_dependency_init(${TARGET_NAME})

set(SHADER_SOURCES
    shader/ShaderTest.hlsl 
)

set(SOURCES
    test/Catch2Tests.cpp
)

set(SHADER_SOURCE_REL_DIR "Shader")
set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shader)

target_sources(${TARGET_NAME} PRIVATE ${SHADER_SOURCES})
target_sources(${TARGET_NAME} PRIVATE ${SOURCES})

set_source_files_properties(${SHADER_SOURCES} PROPERTIES VS_SETTINGS "ExcludedFromBuild=true")
set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${TARGET_NAME}>" )

source_group(TREE ${SHADER_SOURCE_DIR} PREFIX "ShaderSources" FILES ${SHADER_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

target_compile_definitions(${TARGET_NAME} PRIVATE USE_PIX)
target_compile_options(${TARGET_NAME} PRIVATE ${GLOBAL_COMPILE_FLAGS})
target_link_libraries(${TARGET_NAME} PRIVATE ShaderTestFramework)

target_add_asset_directory(${TARGET_NAME} ${SHADER_SOURCE_DIR} "/${SHADER_SOURCE_REL_DIR}")
copy_all_dependent_assets(${TARGET_NAME})

add_catch2(${TARGET_NAME})
