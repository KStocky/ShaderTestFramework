find_package(MSUnitTestFramework REQUIRED)
find_program(VSTest_EXECUTABLE NAMES vstest.console.exe PATHS "${CMAKE_GENERATOR_INSTANCE}/Common7/IDE/Extensions/TestPlatform/" REQUIRED)

get_filename_component(VSTest_EXECUTABLE_DIR ${VSTest_EXECUTABLE} DIRECTORY)

set(AGILITY_DEFINE_PATH_REL_EXE "../../../../../../../../D3D12Agility/")
set(AGILITY_DEFINE_PATH_REL "./D3D12/")
set(AGILITY_DEFINE_PATH "./${AGILITY_DEFINE_PATH_REL}")

set(AGILITY_INSTALL_PATH_REL "${VSTest_EXECUTABLE_DIR}/${AGILITY_DEFINE_PATH_REL_EXE}")
set(AGILITY_INSTALL_PATH_GEN "$<TARGET_FILE_DIR:ShaderTestFrameworkTests>/D3D12/")
set(AGILITY_INSTALL_PATH "${AGILITY_INSTALL_PATH_GEN}")

add_library(ShaderTestFrameworkTests SHARED)
target_sources(ShaderTestFrameworkTests PRIVATE BasicTests.cpp)
target_link_libraries(ShaderTestFrameworkTests PRIVATE ShaderTestFramework MSUnitTestFramework)
target_compile_options(ShaderTestFrameworkTests PRIVATE ${GLOBAL_COMPILE_FLAGS})
target_compile_definitions(ShaderTestFrameworkTests PRIVATE "D3D12_AGILITY_SDK_PATH=\"${AGILITY_DEFINE_PATH}\"")

add_custom_command(TARGET ShaderTestFrameworkTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${AGILITY_SDK_PATH}
        ${AGILITY_INSTALL_PATH}
        COMMENT "Copying Agility SDK files to /D3D12 that is next to tests"
    )


add_test(NAME ShaderTestFrameworkTests COMMAND "${VSTest_EXECUTABLE}" "$<TARGET_FILE:ShaderTestFrameworkTests>")