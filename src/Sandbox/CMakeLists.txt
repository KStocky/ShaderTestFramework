add_executable(Sandbox)
target_sources(Sandbox PRIVATE 
    main.cpp)
target_link_libraries(Sandbox PRIVATE ShaderTestFramework)
target_compile_options(Sandbox PRIVATE ${GLOBAL_COMPILE_FLAGS})

set(AGILITY_SDK_PATH_GENERATOR "$<TARGET_FILE_DIR:Sandbox>/D3D12/")
set(AGILITY_SDK_PATH_REL "./D3D12/")
set(AGILITY_SDK_PATH_DEFINE "${AGILITY_SDK_PATH_REL}")

target_compile_definitions(Sandbox PRIVATE "D3D12_AGILITY_SDK_PATH=\"${AGILITY_SDK_PATH_DEFINE}\"")

add_custom_command(TARGET Sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${AGILITY_SDK_PATH}
        $<TARGET_FILE_DIR:Sandbox>/D3D12
        COMMENT "Copying Agility SDK files to /D3D12 that is next to .exe"
    )