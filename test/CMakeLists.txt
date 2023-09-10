find_package(MSUnitTestFramework REQUIRED)
find_program(VSTest_EXECUTABLE NAMES vstest.console.exe PATHS "${CMAKE_GENERATOR_INSTANCE}/Common7/IDE/Extensions/TestPlatform/" REQUIRED)

add_library(ShaderTestFrameworkTests SHARED)
target_sources(ShaderTestFrameworkTests PRIVATE BasicTests.cpp)
target_link_libraries(ShaderTestFrameworkTests PRIVATE ShaderTestFramework MSUnitTestFramework)
target_compile_options(ShaderTestFrameworkTests PRIVATE ${GLOBAL_COMPILE_FLAGS})
add_test(NAME ShaderTestFrameworkTests COMMAND "${VSTest_EXECUTABLE}" "$<TARGET_FILE:ShaderTestFrameworkTests>")