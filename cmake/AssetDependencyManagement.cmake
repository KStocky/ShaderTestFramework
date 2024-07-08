include_guard(GLOBAL)

function(asset_dependency_init IN_TARGET)
    get_cmake_property(CACHE_VARS CACHE_VARIABLES)
    set(CACHE_VARS_TO_DELETE "TARGET_DIRECTORY_SRC_${IN_TARGET};TARGET_DIRECTORY_DST_${IN_TARGET}")

    foreach(CACHE_VAR IN LISTS CACHE_VARS_TO_DELETE)
        unset(${CACHE_VAR} CACHE)
    endforeach()
endfunction()

function(target_add_asset_directory IN_TARGET IN_ABSOLUTE_SRC IN_RELATIVE_DEST)
    if(DEFINED CACHE{TARGET_DIRECTORY_SRC_${IN_TARGET}})
        set(TARGET_DIRECTORY_SRC_${IN_TARGET} "${TARGET_DIRECTORY_SRC_${IN_TARGET}};${IN_ABSOLUTE_SRC}" CACHE INTERNAL "${IN_TARGET} Src DLL Directories")
        set(TARGET_DIRECTORY_DST_${IN_TARGET} "${TARGET_DIRECTORY_DST_${IN_TARGET}};${IN_RELATIVE_DEST}" CACHE INTERNAL "${IN_TARGET} Dst DLL Directories")
    else()
        set(TARGET_DIRECTORY_SRC_${IN_TARGET} "${IN_ABSOLUTE_SRC}" CACHE INTERNAL "${IN_TARGET} Src DLL Directories")
        set(TARGET_DIRECTORY_DST_${IN_TARGET} "${IN_RELATIVE_DEST}" CACHE INTERNAL "${IN_TARGET} Dst DLL Directories")
    endif()
endfunction()

function(get_all_target_dependencies IN_TARGET IN_OUT_DEPENDENCIES)
    
    get_target_property(ALL_LIBS ${IN_TARGET} LINK_LIBRARIES)
    set(DEPENDENCIES "")
    foreach(LIB IN LISTS ALL_LIBS)
        if (TARGET ${LIB})
            get_all_target_dependencies(${LIB} TARGET_DEPENDENCIES)
            list(APPEND DEPENDENCIES ${LIB} ${TARGET_DEPENDENCIES})
        endif()
    endforeach()

    set(${IN_OUT_DEPENDENCIES} ${DEPENDENCIES})
    return(PROPAGATE ${IN_OUT_DEPENDENCIES})
endfunction()

function(copy_all_dependent_assets IN_TARGET)

    # If we have a multi config generator e.g VS or Ninja Multi-Config
    # We can ensure there are no stale files between builds by just deleting the target directory
    # We can't do that with a single config generator because by default the target is built in place in the
    # target directory
    get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (IS_MULTI_CONFIG)
        set(CLEAN_TARGET "Clean_${IN_TARGET}_Directory")
        add_custom_target(
            ${CLEAN_TARGET}
            ${CMAKE_COMMAND} -E rm -f -r
                "$<TARGET_FILE_DIR:${IN_TARGET}>/"
        )

        add_dependencies(${IN_TARGET} ${CLEAN_TARGET})
    endif()

    get_all_target_dependencies(${IN_TARGET} ALL_LIBS)
    list(APPEND ALL_LIBS ${IN_TARGET})

    foreach(TARGET IN LISTS ALL_LIBS)
        if (DEFINED CACHE{TARGET_DIRECTORY_SRC_${TARGET}})
            foreach(ASSET_DIR IN ZIP_LISTS TARGET_DIRECTORY_SRC_${TARGET} TARGET_DIRECTORY_DST_${TARGET})
                
            add_custom_command(TARGET ${IN_TARGET} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${ASSET_DIR_0}
                    "$<TARGET_FILE_DIR:${IN_TARGET}>${ASSET_DIR_1}")
            endforeach()
        endif()
    endforeach()
endfunction()