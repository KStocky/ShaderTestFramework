include_guard(GLOBAL)

function(dll_dependency_init)
    get_cmake_property(CACHE_VARS CACHE_VARIABLES)

    set(PREFIXES_TO_DELETE "TARGET_DIRECTORY_SRC;TARGET_DIRECTORY_DST")

    string(REGEX MATCHALL "(TARGET_DIRECTORY_SRC|TARGET_DIRECTORY_DST)[A-Za-z_]+" CACHE_VARS_TO_DELETE "${CACHE_VARS}")

    foreach(CACHE_VAR IN LISTS CACHE_VARS_TO_DELETE)
        unset(${CACHE_VAR} CACHE)
    endforeach()
endfunction()

function(target_add_dll_directory IN_TARGET IN_ABSOLUTE_SRC IN_RELATIVE_DEST OUT_SRC_LIST OUT_DST_LIST)

    if(DEFINED CACHE{TARGET_DIRECTORY_SRC_${IN_TARGET}})
        set(TARGET_DIRECTORY_SRC_${IN_TARGET} "${TARGET_DIRECTORY_SRC_${IN_TARGET}};${IN_ABSOLUTE_SRC}" CACHE INTERNAL "${IN_TARGET} Src DLL Directories")
        set(TARGET_DIRECTORY_DST_${IN_TARGET} "${TARGET_DIRECTORY_DST_${IN_TARGET}};${IN_RELATIVE_DEST}" CACHE INTERNAL "${IN_TARGET} Dst DLL Directories")
    else()
        set(TARGET_DIRECTORY_SRC_${IN_TARGET} "${IN_ABSOLUTE_SRC}" CACHE INTERNAL "${IN_TARGET} Src DLL Directories")
        set(TARGET_DIRECTORY_DST_${IN_TARGET} "${IN_RELATIVE_DEST}" CACHE INTERNAL "${IN_TARGET} Dst DLL Directories")
    endif()
    

    set("${OUT_SRC_LIST}" "${TARGET_DIRECTORY_SRC_${IN_TARGET}}" PARENT_SCOPE)
    set("${OUT_DST_LIST}" "${TARGET_DIRECTORY_DST_${IN_TARGET}}" PARENT_SCOPE)
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

function(copy_all_dependent_dlls IN_TARGET)

    add_custom_command(TARGET ${IN_TARGET} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory
        "$<TARGET_FILE_DIR:${IN_TARGET}>/")

    get_all_target_dependencies(${IN_TARGET} ALL_LIBS)
    list(APPEND ALL_LIBS ${IN_TARGET})

    foreach(TARGET IN LISTS ALL_LIBS)
        if (DEFINED CACHE{TARGET_DIRECTORY_SRC_${TARGET}})
            foreach(DLL_DIR IN ZIP_LISTS TARGET_DIRECTORY_SRC_${TARGET} TARGET_DIRECTORY_DST_${TARGET})

                add_custom_command(TARGET ${IN_TARGET} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${DLL_DIR_0}
                    "$<TARGET_FILE_DIR:${IN_TARGET}>${DLL_DIR_1}")
            endforeach()
        endif()
    endforeach()
endfunction()