include("${CMAKE_HOME_DIRECTORY}/cmake/DllDependencyManagement.cmake")

add_library(ShaderTestFramework)
target_sources(ShaderTestFramework PRIVATE 
    include/D3D12AgilityDefinitions.h
    include/EnumReflection.h
    include/ShaderCompiler.h
    include/ShaderEnums.h
    include/ShaderTestFixture.h
    include/Utility.h
    ShaderCompiler.cpp
    ShaderTestFixture.cpp)

target_include_directories(ShaderTestFramework PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include/ )

target_include_directories(ShaderTestFramework PRIVATE ${DXC_INCLUDE_PATH} ${AGILITY_SDK_INCLUDE_PATH} )

target_add_dll_directory(ShaderTestFramework ${AGILITY_SDK_PATH} "/D3D12" SRC_DIRS DEST_DIRS)
target_add_dll_directory(ShaderTestFramework ${WARP_PATH} "/" SRC_DIRS DEST_DIRS)
target_add_dll_directory(ShaderTestFramework ${DXC_PATH} "/" SRC_DIRS DEST_DIRS)

target_link_libraries(ShaderTestFramework PUBLIC d3d12.lib dxgi.lib dxcompiler.lib)

target_compile_options(ShaderTestFramework PRIVATE ${GLOBAL_COMPILE_FLAGS})