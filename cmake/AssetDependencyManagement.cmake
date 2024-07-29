include_guard(GLOBAL)

function(asset_dependency_clean_target IN_TARGET)
    set(TARGET_CLEAN_DIR ${CMAKE_BINARY_DIR}/TargetsToClean)
    if (EXISTS ${TARGET_CLEAN_DIR})

        file(GLOB TARGET_TO_CLEAN_FILES RELATIVE ${TARGET_CLEAN_DIR} "${TARGET_CLEAN_DIR}/*")
        foreach(TARGET_TO_CLEAN_FILE IN ITEMS ${TARGET_TO_CLEAN_FILES})
            file(READ ${TARGET_CLEAN_DIR}/${TARGET_TO_CLEAN_FILE} DIR_TO_DELETE)
            file(REMOVE_RECURSE ${DIR_TO_DELETE})
        endforeach()

        file(REMOVE_RECURSE ${TARGET_CLEAN_DIR})

    endif()
    
    if (NOT EXISTS ${TARGET_CLEAN_DIR})
        file(MAKE_DIRECTORY ${TARGET_CLEAN_DIR})
    endif()
    

    file(GENERATE OUTPUT ${TARGET_CLEAN_DIR}/$<TARGET_NAME:${IN_TARGET}>$<CONFIG> CONTENT "$<TARGET_FILE_DIR:${IN_TARGET}>")
endfunction()

function(asset_dependency_init IN_TARGET)
    get_cmake_property(CACHE_VARS CACHE_VARIABLES)
    set(CACHE_VARS_TO_DELETE "TARGET_DIRECTORY_SRC_${IN_TARGET};TARGET_DIRECTORY_DST_${IN_TARGET}")

    foreach(CACHE_VAR IN LISTS CACHE_VARS_TO_DELETE)
        unset(${CACHE_VAR} CACHE)
    endforeach()

    asset_dependency_clean_target(${IN_TARGET})
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