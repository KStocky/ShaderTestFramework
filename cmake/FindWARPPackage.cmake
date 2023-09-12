set(DX12_WARP_NUPKG_NAME WARP.nupkg)
set(DX12_WARP_ZIP_NAME WARP.zip)
set(WARP_SCRATCH_FOLDER "${CMAKE_CURRENT_BINARY_DIR}")
set(WARP_PKG_PATH "${WARP_SCRATCH_FOLDER}/${DX12_WARP_NUPKG_NAME}")
set(WARP_ZIP_PATH "${WARP_SCRATCH_FOLDER}/${DX12_WARP_ZIP_NAME}")
set(WARP_EXTRACT_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/extern/WARP")
set(WARP_PATH ${WARP_EXTRACT_FOLDER} CACHE STRING "WARP Path")
set(WARP_FOUND FALSE)

if (NOT EXISTS ${WARP_PKG_PATH})

    message(INFO "Downloading DirectX 12 WARP nuget package...")

    file(DOWNLOAD
        https://www.nuget.org/api/v2/package/Microsoft.Direct3D.WARP/1.0.7.1
        ${WARP_PKG_PATH}
        STATUS DOWNLOAD_STATUS
    )

    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)

    if (${STATUS_CODE} EQUAL 0)

        message(STATUS "WARP download complete!")

    else()

        message(WARNING "Download was unsuccessful. Error: ${ERROR_MESSAGE}")

    endif()

    if (NOT EXISTS ${WARP_PKG_PATH})

        message(WARNING "Could not download DirectX 12 WARP nuget package. Project will load system WARP instead.")

    endif()

endif()

if (EXISTS ${WARP_PKG_PATH})

    message(STATUS "Extracting WARP nuget package contents!")

    # Convert the file from .nupkg to .zip to extract its contents
    configure_file(${WARP_PKG_PATH} ${WARP_ZIP_PATH} COPYONLY)

    file(MAKE_DIRECTORY  ${WARP_EXTRACT_FOLDER})

    # Extract contents from .zip file to created directory
    execute_process(
        COMMAND ${CMAKE_COMMAND} "-E" "tar" "xvz" "${WARP_ZIP_PATH}"
        WORKING_DIRECTORY ${WARP_EXTRACT_FOLDER}
    )

    set(WARP_PATH "${WARP_EXTRACT_FOLDER}/build/native/amd64")

    set(WARP_FOUND TRUE)

endif()