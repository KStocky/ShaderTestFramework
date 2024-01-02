include_guard(GLOBAL)

function(nuget_initialize IN_TARGET)
    set(NUGET_ROOT_DIR_${IN_TARGET} ${CMAKE_CURRENT_SOURCE_DIR}/nuget CACHE INTERNAL "Directory where nuget packages are installed")
endfunction()

function(nuget_get_pkg_ver IN_TARGET IN_PKG_NAME OUT_VERSION)
    
    set(PKG_DIR "${NUGET_ROOT_DIR_${IN_TARGET}}/${IN_PKG_NAME}")
    file(GLOB CHILDREN RELATIVE "${PKGDIR}" "${PKG_DIR}/*")
    set(${OUT_VERSION} "")
    foreach(CHILD IN LISTS CHILDREN)
        if(IS_DIRECTORY ${CHILD})
            cmake_path(GET CHILD FILENAME VERSION)
            set(${OUT_VERSION} ${VERSION})
            break()
        endif()
    endforeach()

    return(PROPAGATE ${OUT_VERSION})
endfunction()

function(nuget_pkg_already_installed IN_TARGET IN_PKG_NAME OUT_RESULT OUT_VERSION)
    
    if (NOT DEFINED CACHE{NUGET_ROOT_DIR_${IN_TARGET}})
        set(${OUT_RESULT} FALSE)
        return(PROPAGATE ${OUT_RESULT})
    endif()

    set(PKG_DIR "${NUGET_ROOT_DIR_${IN_TARGET}}/${IN_PKG_NAME}")
    
    if (EXISTS ${PKG_DIR})
        set(${OUT_RESULT} TRUE)
        nuget_get_pkg_ver(${IN_TARGET} ${IN_PKG_NAME} ${OUT_VERSION})

        return(PROPAGATE ${OUT_RESULT} ${OUT_VERSION})
    else()
        set(${OUT_VERSION} "")
        set(${OUT_RESULT} FALSE)
        return(PROPAGATE ${OUT_RESULT} ${OUT_VERSION})
    endif()

endfunction()

function(nuget_pkg_get IN_TARGET IN_PKG_NAME IN_VERSION OUT_SUCCEEDED OUT_PKG_PATH)

    set(PKG_DIR "${NUGET_ROOT_DIR_${IN_TARGET}}/${IN_PKG_NAME}")

    nuget_pkg_already_installed(${IN_TARGET} ${IN_PKG_NAME} PKG_EXISTS CURRENT_VERSION)

    if (${PKG_EXISTS})
        if(CURRENT_VERSION STRGREATER_EQUAL IN_VERSION)
            if(CURRENT_VERSION STRGREATER IN_VERSION)
                message("Package ${IN_PKG_NAME} already has a more up to date version installed. Current: ${CURRENT_VERSION} Requested: ${IN_VERSION}")
            endif()

            set(${OUT_SUCCEEDED} TRUE)
            set(${OUT_PKG_PATH} "${PKG_DIR}/${CURRENT_VERSION}")
            return(PROPAGATE ${OUT_SUCCEEDED} ${OUT_PKG_PATH})
        endif()
    endif()

    message(INFO "Downloading ${IN_PKG_NAME} nuget package...")

    set(SCRATCH_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    set(SCRATCH_PKG "${SCRATCH_DIR}/${IN_PKG_NAME}${VERSION}.nupkg")
    file(DOWNLOAD
        https://www.nuget.org/api/v2/package/${IN_PKG_NAME}/${IN_VERSION}
        ${SCRATCH_PKG}
        STATUS DOWNLOAD_STATUS
    )

    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)

    if (${STATUS_CODE} EQUAL 0)

        message(STATUS "${IN_PKG_NAME} download complete!")
        if (${PKG_EXISTS} AND CURRENT_VERSION STRLESS IN_VERSION)
            message("Deleting old version ${CURRENT_VERSION}")
            file(REMOVE_RECURSE ${PKG_DIR})
        endif()
    else()

        message("Download was unsuccessful. Error: ${ERROR_MESSAGE}")
        set(${OUT_SUCCEEDED} ${PKG_EXISTS}})
        set(${OUT_PKG_PATH} "${PKG_DIR}/CURRENT_VERSION}")
        return(PROPAGATE ${OUT_SUCCEEDED} ${OUT_PKG_PATH})
    endif()

    message(STATUS "Extracting Agility SDK nuget package contents!")

    set(SCRATCH_ZIP "${SCRATCH_DIR}/${IN_PKG_NAME}${VERSION}.zip")
    # Convert the file from .nupkg to .zip to extract its contents
    configure_file(${SCRATCH_PKG} ${SCRATCH_ZIP} COPYONLY)

    set(PKG_VER_DIR "${PKG_DIR}/${IN_VERSION}")
    file(MAKE_DIRECTORY ${PKG_VER_DIR})

    # Extract contents from .zip file to created directory
    execute_process(
        COMMAND ${CMAKE_COMMAND} "-E" "tar" "xvz" "${SCRATCH_ZIP}"
        WORKING_DIRECTORY ${PKG_VER_DIR}
    )

    #Delete scratch files
    file(REMOVE ${SCRATCH_PKG})
    file(REMOVE ${SCRATCH_ZIP})

    set(${OUT_SUCCEEDED} TRUE)
    set(${OUT_PKG_PATH} ${PKG_VER_DIR})
    return(PROPAGATE ${OUT_SUCCEEDED} ${OUT_PKG_PATH})

endfunction()