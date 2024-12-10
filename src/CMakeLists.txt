include(AssetDependencyManagement)
include(ExternalProjects)
include(NugetPackages)

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
    Public/D3D12/Shader/ShaderBinding.h
    Public/D3D12/Shader/ShaderCompiler.h
    Public/D3D12/Shader/ShaderEnums.h
    Public/D3D12/Shader/ShaderHash.h
    Public/D3D12/Shader/ShaderReflectionUtils.h
    Public/D3D12/Shader/VirtualShaderDirectoryMapping.h
    Public/D3D12/Shader/VirtualShaderDirectoryMappingManager.h
    Public/Framework/HLSLTypes.h
    Public/Framework/PIXCapturer.h
    Public/Framework/ShaderTestCommon.h
    Public/Framework/ShaderTestFixture.h
    Public/Framework/ShaderTestShader.h
    Public/Framework/TestDataBufferLayout.h
    Public/Framework/TestDataBufferProcessor.h
    Public/Framework/TestDataBufferStructs.h
    Public/Framework/TypeByteReader.h
    Public/Platform.h
    Public/Stats/ScopedObject.h
    Public/Stats/StatSystem.h
    Public/Utility/Algorithm.h
    Public/Utility/Concepts.h
    Public/Utility/EnumReflection.h
    Public/Utility/Exception.h
    Public/Utility/Expected.h
    Public/Utility/FixedString.h
    Public/Utility/Float.h
    Public/Utility/FunctionTraits.h
    Public/Utility/Lambda.h
    Public/Utility/Math.h
    Public/Utility/MoveOnly.h
    Public/Utility/Object.h
    Public/Utility/OverloadSet.h
    Public/Utility/Pointer.h
    Public/Utility/Time.h
    Public/Utility/Tuple.h
    Public/Utility/Type.h
    Public/Utility/TypeList.h
    Public/Utility/TypeTraits.h
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
    Private/D3D12/Shader/ShaderBinding.cpp
    Private/D3D12/Shader/ShaderCompiler.cpp
    Private/D3D12/Shader/ShaderReflectionUtils.cpp
    Private/D3D12/Shader/VirtualShaderDirectoryMappingManager.cpp
    Private/Framework/PIXCapturer.cpp
    Private/Framework/ShaderTestCommon.cpp
    Private/Framework/ShaderTestFixture.cpp
    Private/Framework/ShaderTestShader.cpp
    Private/Framework/TestDataBufferLayout.cpp
    Private/Framework/TestDataBufferProcessor.cpp
    Private/Framework/TestDataBufferStructs.cpp
    Private/Framework/TypeByteReader.cpp
    Private/Stats/StatSystem.cpp
)

set(SHADER_SOURCES
    Shader/STF/ByteReaderTraits.hlsli
    Shader/STF/FrameworkResources.hlsli
    Shader/STF/SectionManagement.hlsli
    Shader/STF/ShaderTestFramework.hlsli
    Shader/TTL/byte_writer.hlsli
    Shader/TTL/caster.hlsli
    Shader/TTL/concepts.hlsli
    Shader/TTL/container_wrapper.hlsli
    Shader/TTL/macro.hlsli
    Shader/TTL/memory.hlsli
    Shader/TTL/models.hlsli
    Shader/TTL/string.hlsli
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

# Hide away unity files in their own filter
source_group(UnityBuildFiles REGULAR_EXPRESSION ".+_cxx.cxx")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source" FILES ${SOURCES})
source_group(TREE ${AGILITY_INCLUDE} PREFIX "ThirdParty/AgilitySDK" FILES ${AGILITY_HEADERS})
source_group(TREE ${DXC_INCLUDE} PREFIX "ThirdParty/DXC" FILES ${DXC_HEADERS})
source_group(TREE ${WINPIX_INCLUDE} PREFIX "ThirdParty/WinPix" FILES ${WINPIX_HEADERS})
source_group(TREE ${SHADER_SOURCE_DIR} PREFIX "ShaderSources" FILES ${SHADER_SOURCES})

target_compile_definitions(ShaderTestFramework PRIVATE STF_D3D12_SDK_VERSION_ID=${SDK_VER} SHADER_SRC="${SHADER_SOURCE_REL_DIR}" USE_PIX)

# Silencing this while we are using a float16 library.
# We can remove this when we start using std::float16_t whenever that is implemented...
target_compile_definitions(ShaderTestFramework PUBLIC _SILENCE_CXX23_DENORM_DEPRECATION_WARNING NOMINMAX)
target_compile_options(ShaderTestFramework PRIVATE ${GLOBAL_COMPILE_FLAGS})
target_include_directories(ShaderTestFramework PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Public/ ${AGILITY_INCLUDE} ${DXC_INCLUDE} ${WINPIX_INCLUDE})
target_link_libraries(ShaderTestFramework PRIVATE d3d12.lib dxgi.lib DXC dxguid.lib WINPIX)
if (${STF_DISABLE_UNITY_STF})
    set_target_properties(ShaderTestFramework PROPERTIES UNITY_BUILD OFF)
endif()

add_tuplet(ShaderTestFramework)
add_float16(ShaderTestFramework)

target_add_asset_directory(ShaderTestFramework ${AGILITY_DLL} "/D3D12")
target_add_asset_directory(ShaderTestFramework ${WARP_DLL} "/")
target_add_asset_directory(ShaderTestFramework ${DXC_DLL} "/")
target_add_asset_directory(ShaderTestFramework ${WINPIX_BIN} "/")
target_add_asset_directory(ShaderTestFramework ${SHADER_SOURCE_DIR} "/${SHADER_SOURCE_REL_DIR}")