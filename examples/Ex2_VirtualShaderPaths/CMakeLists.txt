# Library for copying asset files to the final working directory of the exe
include(AssetDependencyManagement)

#Library for including external projects e.g. catch2
include(ExternalProjects)

add_executable(Ex2_VirtualShaderPaths VirtualShaderPaths.cpp)

# init Asset dependency library for this example project
asset_dependency_init(Ex2_VirtualShaderPaths)

# Shader source is given as the absolute path to the shader source directory
set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ShaderCode)
# Shader destination is given as the relative path from the destination executable
set(SHADER_DEST_DIR "./ShaderCode")

# Set a define that can be queried in the test code for where the shader code lives
target_compile_definitions(Ex2_VirtualShaderPaths PRIVATE SHADER_SRC="${SHADER_DEST_DIR}")

# Add the shader code directory to the list of directories that will be copied after building
# The second argument is the location of shader source.
# The third argument is where we what that directory to be copied to.
target_add_asset_directory(Ex2_VirtualShaderPaths ${SHADER_SOURCE_DIR} ${SHADER_DEST_DIR})

# Link this example project with the shader test framework
target_link_libraries(Ex2_VirtualShaderPaths PRIVATE ShaderTestFramework)

# Set the C++ standard to C++23
set_property(TARGET Ex2_VirtualShaderPaths PROPERTY CXX_STANDARD 23)

# Ensure that the working directory of the example project is the directory of the executable
set_target_properties(Ex2_VirtualShaderPaths PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Ex2_VirtualShaderPaths>" )

# Copies all assets that this project depends upon to the working directory
copy_all_dependent_assets(Ex2_VirtualShaderPaths)

# Adds catch2 to the project as its testing framework.
add_catch2(Ex2_VirtualShaderPaths)
