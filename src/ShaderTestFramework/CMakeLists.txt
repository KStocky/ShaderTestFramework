include("${CMAKE_HOME_DIRECTORY}/cmake/DllDependencyManagement.cmake")
include("${CMAKE_HOME_DIRECTORY}/cmake/ExternalProjects.cmake")
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
    Public/D3D12/CommandAllocator.h
    Public/D3D12/CommandEngine.h
    Public/D3D12/CommandList.h
    Public/D3D12/CommandQueue.h
    Public/D3D12/Descriptor.h
    Public/D3D12/DescriptorFreeListAllocator.h
    Public/D3D12/DescriptorHeap.h
    Public/D3D12/DescriptorRingAllocator.h
    Public/D3D12/Fence.h
    Public/D3D12/GPUDevice.h
    Public/D3D12/GPUResource.h
    Public/D3D12/Shader/CompiledShaderData.h
    Public/D3D12/Shader/RootSignature.h
    Public/D3D12/Shader/ShaderCompiler.h
    Public/D3D12/Shader/ShaderEnums.h
    Public/D3D12/Shader/ShaderHash.h
    Public/Platform.h
    Public/ShaderTestFixture.h
    Public/Utility/Algorithm.h
    Public/Utility/Concepts.h
    Public/Utility/EnumReflection.h
    Public/Utility/Exception.h
    Public/Utility/Expected.h
    Public/Utility/FixedString.h
    Public/Utility/FunctionTraits.h
    Public/Utility/Lambda.h
    Public/Utility/MoveOnly.h
    Public/Utility/OverloadSet.h
    Public/Utility/Pointer.h
    Public/Utility/Tuple.h
    Public/Utility/Type.h
    Public/Utility/TypeList.h
    Private/D3D12/CommandAllocator.cpp
    Private/D3D12/CommandEngine.cpp
    Private/D3D12/CommandList.cpp
    Private/D3D12/CommandQueue.cpp
    Private/D3D12/Descriptor.cpp
    Private/D3D12/DescriptorFreeListAllocator.cpp
    Private/D3D12/DescriptorHeap.cpp
    Private/D3D12/DescriptorRingAllocator.cpp
    Private/D3D12/Fence.cpp
    Private/D3D12/GPUDevice.cpp
    Private/D3D12/GPUResource.cpp
    Private/D3D12/Shader/CompiledShaderData.cpp
    Private/D3D12/Shader/RootSignature.cpp
    Private/D3D12/Shader/ShaderCompiler.cpp
    Private/ShaderTestFixture.cpp)

file(GLOB_RECURSE AGILITY_HEADERS "${AGILITY_INCLUDE}/*.h*" )
file(GLOB_RECURSE DXC_HEADERS "${DXC_INCLUDE}/*.h*" )

target_sources(ShaderTestFramework PRIVATE 
    ${SOURCES})

target_sources(ShaderTestFramework PRIVATE 
    ${AGILITY_HEADERS})

target_sources(ShaderTestFramework PRIVATE 
    ${DXC_HEADERS})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source" FILES ${SOURCES})
source_group(TREE ${AGILITY_INCLUDE} PREFIX "ThirdParty/AgilitySDK" FILES ${AGILITY_HEADERS})
source_group(TREE ${DXC_INCLUDE} PREFIX "ThirdParty/DXC" FILES ${DXC_HEADERS})

target_include_directories(ShaderTestFramework PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Public/ )

target_include_directories(ShaderTestFramework PUBLIC ${AGILITY_INCLUDE} ${DXC_INCLUDE} )

target_compile_definitions(ShaderTestFramework PRIVATE D3D12_SDK_VERSION_ID=${SDK_VER})

target_add_dll_directory(ShaderTestFramework ${AGILITY_DLL} "/D3D12" SRC_DIRS DEST_DIRS)
target_add_dll_directory(ShaderTestFramework ${WARP_DLL} "/" SRC_DIRS DEST_DIRS)
target_add_dll_directory(ShaderTestFramework ${DXC_DLL} "/" SRC_DIRS DEST_DIRS)

target_link_libraries(ShaderTestFramework PRIVATE d3d12.lib dxgi.lib DXC dxguid.lib)

target_compile_options(ShaderTestFramework PRIVATE ${GLOBAL_COMPILE_FLAGS})

add_tl_expected(ShaderTestFramework)
add_tuplet(ShaderTestFramework)