include("${CMAKE_HOME_DIRECTORY}/cmake/AssetDependencyManagement.cmake")
include("${CMAKE_HOME_DIRECTORY}/cmake/ExternalProjects.cmake")
include("${CMAKE_HOME_DIRECTORY}/cmake/NugetPackages.cmake")

add_library(ShaderTestFramework)

nuget_initialize(ShaderTestFramework)
nuget_get_agility_sdk(ShaderTestFramework AGILITY_FOUND AGILITY_INCLUDE AGILITY_DLL SDK_VER)
nuget_get_dxc(ShaderTestFramework DXC_FOUND DXC_INCLUDE DXC_DLL DXC_LIB)
nuget_get_warp(ShaderTestFramework WARP_FOUND WARP_DLL)
nuget_get_WinPixEventRuntime(ShaderTestFramework WINPIX_FOUND WINPIX_INCLUDE WINPIX_BIN)

add_library(DXC SHARED IMPORTED)
set_target_properties(DXC PROPERTIES
    IMPORTED_IMPLIB "${DXC_LIB}/dxcompiler.lib"
)

add_library(WINPIX SHARED IMPORTED)
set_target_properties(WINPIX PROPERTIES
    IMPORTED_IMPLIB "${WINPIX_BIN}/WinPixEventRuntime.lib")

asset_dependency_init(ShaderTestFramework)

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
    Public/D3D12/Shader/IncludeHandler.h
    Public/D3D12/Shader/PipelineState.h
    Public/D3D12/Shader/RootSignature.h
    Public/D3D12/Shader/ShaderCompiler.h
    Public/D3D12/Shader/ShaderEnums.h
    Public/D3D12/Shader/ShaderHash.h
    Public/D3D12/Shader/VirtualShaderDirectoryMapping.h
    Public/D3D12/Shader/VirtualShaderDirectoryMappingManager.h
    Public/Framework/AssertBufferProcessor.h
    Public/Framework/HLSLTypes.h
    Public/Framework/PIXCapturer.h
    Public/Framework/ShaderTestFixture.h
    Public/Platform.h
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
    Private/D3D12/DescriptorFreeListAllocator.cpp
    Private/D3D12/DescriptorHeap.cpp
    Private/D3D12/DescriptorRingAllocator.cpp
    Private/D3D12/Fence.cpp
    Private/D3D12/GPUDevice.cpp
    Private/D3D12/GPUResource.cpp
    Private/D3D12/Shader/CompiledShaderData.cpp
    Private/D3D12/Shader/IncludeHandler.cpp
    Private/D3D12/Shader/PipelineState.cpp
    Private/D3D12/Shader/RootSignature.cpp
    Private/D3D12/Shader/ShaderCompiler.cpp
    Private/D3D12/Shader/VirtualShaderDirectoryMappingManager.cpp
    Private/Framework/AssertBufferProcessor.cpp
    Private/Framework/PIXCapturer.cpp
    Private/Framework/ShaderTestFixture.cpp
)

set(SHADER_SOURCES
    Shader/STF/RootSignature.hlsli
    Shader/STF/SectionManagement.hlsli
    Shader/STF/ShaderTestFramework.hlsli
    Shader/STF/TypeId.hlsli
    Shader/TTL/byte_writer.hlsli
    Shader/TTL/caster.hlsli
    Shader/TTL/container_wrapper.hlsli
    Shader/TTL/type_traits.hlsli
)

set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shader)
set(SHADER_SOURCE_REL_DIR "Shaders")

file(GLOB_RECURSE AGILITY_HEADERS "${AGILITY_INCLUDE}/*.h*" )
file(GLOB_RECURSE DXC_HEADERS "${DXC_INCLUDE}/*.h*" )
file(GLOB_RECURSE WINPIX_HEADERS "${WINPIX_INCLUDE}/*.h*" )

target_sources(ShaderTestFramework PRIVATE ${SOURCES})
target_sources(ShaderTestFramework PRIVATE ${AGILITY_HEADERS})
target_sources(ShaderTestFramework PRIVATE ${DXC_HEADERS})
target_sources(ShaderTestFramework PRIVATE ${WINPIX_HEADERS})
target_sources(ShaderTestFramework PRIVATE ${SHADER_SOURCES})

set_source_files_properties(${SHADER_SOURCES} PROPERTIES VS_SETTINGS "ExcludedFromBuild=true")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source" FILES ${SOURCES})
source_group(TREE ${AGILITY_INCLUDE} PREFIX "ThirdParty/AgilitySDK" FILES ${AGILITY_HEADERS})
source_group(TREE ${DXC_INCLUDE} PREFIX "ThirdParty/DXC" FILES ${DXC_HEADERS})
source_group(TREE ${WINPIX_INCLUDE} PREFIX "ThirdParty/WinPix" FILES ${WINPIX_HEADERS})
source_group(TREE ${SHADER_SOURCE_DIR} PREFIX "ShaderSources" FILES ${SHADER_SOURCES})

target_compile_definitions(ShaderTestFramework PRIVATE D3D12_SDK_VERSION_ID=${SDK_VER} SHADER_SRC="${SHADER_SOURCE_REL_DIR}" USE_PIX)
target_compile_options(ShaderTestFramework PRIVATE ${GLOBAL_COMPILE_FLAGS})
target_include_directories(ShaderTestFramework PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Public/ ${AGILITY_INCLUDE} ${DXC_INCLUDE} ${WINPIX_INCLUDE})
target_link_libraries(ShaderTestFramework PRIVATE d3d12.lib dxgi.lib DXC dxguid.lib WINPIX)
add_tl_expected(ShaderTestFramework)
add_tuplet(ShaderTestFramework)

target_add_asset_directory(ShaderTestFramework ${AGILITY_DLL} "/D3D12")
target_add_asset_directory(ShaderTestFramework ${WARP_DLL} "/")
target_add_asset_directory(ShaderTestFramework ${DXC_DLL} "/")
target_add_asset_directory(ShaderTestFramework ${WINPIX_BIN} "/")
target_add_asset_directory(ShaderTestFramework ${SHADER_SOURCE_DIR} "/${SHADER_SOURCE_REL_DIR}")