include("${CMAKE_HOME_DIRECTORY}/cmake/DllDependencyManagement.cmake")
include("${CMAKE_HOME_DIRECTORY}/cmake/NugetPackages.cmake")

nuget_get_agility_sdk(AGILITY_FOUND AGILITY_INCLUDE AGILITY_DLL)
nuget_get_dxc(DXC_FOUND DXC_INCLUDE DXC_DLL DXC_LIB)
nuget_get_warp(WARP_FOUND WARP_DLL)

add_library(DXC SHARED IMPORTED)
set_target_properties(DXC PROPERTIES
    IMPORTED_IMPLIB "${DXC_LIB}/dxcompiler.lib"
)

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

target_include_directories(ShaderTestFramework PRIVATE ${AGILITY_INCLUDE} ${DXC_INCLUDE} )

target_add_dll_directory(ShaderTestFramework ${AGILITY_DLL} "/D3D12" SRC_DIRS DEST_DIRS)
target_add_dll_directory(ShaderTestFramework ${WARP_DLL} "/" SRC_DIRS DEST_DIRS)
target_add_dll_directory(ShaderTestFramework ${DXC_DLL} "/" SRC_DIRS DEST_DIRS)

target_link_libraries(ShaderTestFramework PRIVATE d3d12.lib dxgi.lib DXC)

target_compile_options(ShaderTestFramework PRIVATE ${GLOBAL_COMPILE_FLAGS})