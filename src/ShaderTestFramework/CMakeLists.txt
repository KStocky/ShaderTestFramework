include("${CMAKE_HOME_DIRECTORY}/cmake/DllDependencyManagement.cmake")
include("${CMAKE_HOME_DIRECTORY}/cmake/NugetPackages.cmake")

nuget_get_agility_sdk(AGILITY_FOUND AGILITY_INCLUDE AGILITY_DLL SDK_VER)
nuget_get_dxc(DXC_FOUND DXC_INCLUDE DXC_DLL DXC_LIB)
nuget_get_warp(WARP_FOUND WARP_DLL)

add_library(DXC SHARED IMPORTED)
set_target_properties(DXC PROPERTIES
    IMPORTED_IMPLIB "${DXC_LIB}/dxcompiler.lib"
)

add_library(ShaderTestFramework)

set(SOURCES
    Public/Container/RingBuffer.h
    Public/D3D12/AgilityDefinitions.h
    Public/D3D12/Descriptor.h
    Public/D3D12/DescriptorFreeListAllocator.h
    Public/D3D12/DescriptorHeap.h
    Public/D3D12/DescriptorRingAllocator.h
    Public/D3D12/Fence.h
    Public/D3D12/GPUDevice.h
    Public/Exception.h
    Public/EnumReflection.h
    Public/MoveOnly.h
    Public/Platform.h
    Public/Pointer.h
    Public/ShaderCompiler.h
    Public/ShaderEnums.h
    Public/ShaderTestFixture.h
    Public/Utility.h
    Private/D3D12/Descriptor.cpp
    Private/D3D12/DescriptorFreeListAllocator.cpp
    Private/D3D12/DescriptorHeap.cpp
    Private/D3D12/DescriptorRingAllocator.cpp
    Private/D3D12/Fence.cpp
    Private/D3D12/GPUDevice.cpp
    Private/D3D12/ShaderCompiler.cpp
    Private/ShaderTestFixture.cpp)

target_sources(ShaderTestFramework PRIVATE 
    ${SOURCES})

target_include_directories(ShaderTestFramework PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Public/ )

target_include_directories(ShaderTestFramework PRIVATE ${AGILITY_INCLUDE} ${DXC_INCLUDE} )

target_compile_definitions(ShaderTestFramework PRIVATE D3D12_SDK_VERSION_ID=${SDK_VER})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

target_add_dll_directory(ShaderTestFramework ${AGILITY_DLL} "/D3D12" SRC_DIRS DEST_DIRS)
target_add_dll_directory(ShaderTestFramework ${WARP_DLL} "/" SRC_DIRS DEST_DIRS)
target_add_dll_directory(ShaderTestFramework ${DXC_DLL} "/" SRC_DIRS DEST_DIRS)

target_link_libraries(ShaderTestFramework PRIVATE d3d12.lib dxgi.lib DXC dxguid.lib)

target_compile_options(ShaderTestFramework PRIVATE ${GLOBAL_COMPILE_FLAGS})