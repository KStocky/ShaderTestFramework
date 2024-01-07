# Library for copying asset files to the final working directory of the exe
include(AssetDependencyManagement)

#Library for including external projects e.g. catch2
include(ExternalProjects)

add_executable(MinimalShaderTest MinimalShaderTest.cpp)

# init Asset dependency library for this example project
asset_dependency_init(MinimalShaderTest)

# Link this example project with the shader test framework
target_link_libraries(MinimalShaderTest PRIVATE ShaderTestFramework)

# Set the C++ standard to C++23
set_property(TARGET MinimalShaderTest MinimalShaderTest PROPERTY CXX_STANDARD 23)

# Ensure that the working directory of the example project is the directory of the executable
set_target_properties(MinimalShaderTest PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:MinimalShaderTest>" )

# Copies all assets that this project depends upon to the working directory
copy_all_dependent_assets(MinimalShaderTest)

# Adds catch2 to the project as its testing framework.
add_catch2(MinimalShaderTest)
