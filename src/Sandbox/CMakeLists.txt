include("${CMAKE_HOME_DIRECTORY}/cmake/DllDependencyManagement.cmake")

add_executable(Sandbox)
target_sources(Sandbox PRIVATE 
    main.cpp)
target_link_libraries(Sandbox PRIVATE ShaderTestFramework)
target_compile_options(Sandbox PRIVATE ${GLOBAL_COMPILE_FLAGS})

set(AGILITY_SDK_PATH_REL "./D3D12/")
set(AGILITY_SDK_PATH_DEFINE "${AGILITY_SDK_PATH_REL}")

target_compile_definitions(Sandbox PRIVATE "D3D12_AGILITY_SDK_PATH=\"${AGILITY_SDK_PATH_DEFINE}\"")

copy_all_dependent_dlls(Sandbox)
