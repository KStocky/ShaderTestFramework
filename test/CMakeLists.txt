find_package(Catch2 REQUIRED)

add_executable(ShaderTestFrameworkTests)
target_sources(ShaderTestFrameworkTests PRIVATE BasicTests.cpp)
target_link_libraries(ShaderTestFrameworkTests PRIVATE ShaderTestFramework Catch2::Catch2WithMain)
target_compile_options(ShaderTestFrameworkTests PRIVATE ${GLOBAL_COMPILE_FLAGS})

set(AGILITY_INSTALL_PATH $<TARGET_FILE_DIR:ShaderTestFrameworkTests>/D3D12)
set(WARP_INSTALL_PATH $<TARGET_FILE_DIR:ShaderTestFrameworkTests>)
set(DXC_INSTALL_PATH $<TARGET_FILE_DIR:ShaderTestFrameworkTests>)

add_custom_command(TARGET ShaderTestFrameworkTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${AGILITY_SDK_PATH}
        ${AGILITY_INSTALL_PATH}
        COMMENT "Copying Agility SDK files to /D3D12 that is next to tests"
    )

add_custom_command(TARGET ShaderTestFrameworkTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${WARP_PATH}
        ${WARP_INSTALL_PATH}
        COMMENT "Copying WARP to tests"
    )

add_custom_command(TARGET ShaderTestFrameworkTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${DXC_PATH}
    ${DXC_INSTALL_PATH}
    COMMENT "Copying DXC to tests"
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(Catch)

catch_discover_tests(ShaderTestFrameworkTests)
