# Library for copying asset files to the final working directory of the exe
include(AssetDependencyManagement)

#Library for including external projects e.g. catch2
include(ExternalProjects)

add_executable(Ex0_MinimalShaderTest MinimalShaderTest.cpp)

# init Asset dependency library for this example project
asset_dependency_init(Ex0_MinimalShaderTest)

# Link this example project with the shader test framework
target_link_libraries(Ex0_MinimalShaderTest PRIVATE ShaderTestFramework)

# Set the C++ standard to C++23
set_property(TARGET Ex0_MinimalShaderTest PROPERTY CXX_STANDARD 23)

# Ensure that the working directory of the example project is the directory of the executable
set_target_properties(Ex0_MinimalShaderTest PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Ex0_MinimalShaderTest>" )

# Copies all assets that this project depends upon to the working directory
copy_all_dependent_assets(Ex0_MinimalShaderTest)

# Adds catch2 to the project as its testing framework.
add_catch2(Ex0_MinimalShaderTest)
