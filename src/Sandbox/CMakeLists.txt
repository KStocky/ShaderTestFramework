include("${CMAKE_HOME_DIRECTORY}/cmake/AssetDependencyManagement.cmake")

add_executable(Sandbox)
target_sources(Sandbox PRIVATE 
    main.cpp)
target_link_libraries(Sandbox PRIVATE ShaderTestFramework)
target_compile_options(Sandbox PRIVATE ${GLOBAL_COMPILE_FLAGS})

target_compile_definitions(Sandbox PRIVATE "D3D12_AGILITY_SDK_PATH=\"./D3D12/\"")

copy_all_dependent_dlls(Sandbox)
