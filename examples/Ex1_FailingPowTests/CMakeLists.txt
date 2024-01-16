# Library for copying asset files to the final working directory of the exe
include(AssetDependencyManagement)

#Library for including external projects e.g. catch2
include(ExternalProjects)

set(TARGET_NAME Ex1_FailingPowTests)

add_executable(${TARGET_NAME} PowTests.cpp)

# init Asset dependency library for this example project
asset_dependency_init(${TARGET_NAME} )

# Link this example project with the shader test framework
target_link_libraries(${TARGET_NAME}  PRIVATE ShaderTestFramework)

# Set the C++ standard to C++23
set_property(TARGET ${TARGET_NAME}  PROPERTY CXX_STANDARD 23)

# Ensure that the working directory of the example project is the directory of the executable
set_target_properties(${TARGET_NAME}  PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${TARGET_NAME}>" )

# Copies all assets that this project depends upon to the working directory
copy_all_dependent_assets(${TARGET_NAME} )

# Adds catch2 to the project as its testing framework.
add_catch2(${TARGET_NAME} )
