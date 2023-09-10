add_library(ShaderTestFramework)
target_sources(ShaderTestFramework PRIVATE 
    include/ShaderTestFixture.h
    include/D3D12AgilityDefinitions.h
    ShaderTestFixture.cpp)
target_include_directories(ShaderTestFramework PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include/)
target_link_libraries(ShaderTestFramework PUBLIC d3d12.lib dxgi.lib)
target_compile_options(ShaderTestFramework PRIVATE ${GLOBAL_COMPILE_FLAGS})

add_custom_command(TARGET ShaderTestFramework POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${AGILITY_SDK_PATH}
        $<TARGET_FILE_DIR:ShaderTestFramework>/D3D12
        COMMENT "Copying Agility SDK files to /D3D12 that is next to .lib"
    )

    add_definitions(-DAGILITY_SDK)