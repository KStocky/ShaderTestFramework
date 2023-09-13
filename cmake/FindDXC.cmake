set(DX12_DXC_NUPKG_NAME DXC.nupkg)
set(DX12_DXC_ZIP_NAME DXC.zip)
set(DXC_SCRATCH_FOLDER "${CMAKE_CURRENT_BINARY_DIR}")
set(DXC_PKG_PATH "${DXC_SCRATCH_FOLDER}/${DX12_DXC_NUPKG_NAME}")
set(DXC_ZIP_PATH "${DXC_SCRATCH_FOLDER}/${DX12_DXC_ZIP_NAME}")
set(DXC_EXTRACT_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/extern/DXC")
set(DXC_PATH ${DXC_EXTRACT_FOLDER} CACHE STRING "DXC Path")
set(DXC_INCLUDE_PATH ${DXC_EXTRACT_FOLDER}/build/native/include CACHE STRING "DXC Include Path")
set(DXC_FOUND FALSE)

if (NOT EXISTS ${DXC_PKG_PATH})

    message(INFO "Downloading DirectX 12 DXC nuget package...")

    file(DOWNLOAD
        https://www.nuget.org/api/v2/package/Microsoft.Direct3D.DXC
        ${DXC_PKG_PATH}
        STATUS DOWNLOAD_STATUS
    )

    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)

    if (${STATUS_CODE} EQUAL 0)

        message(STATUS "DXC download complete!")

    else()

        message(WARNING "Download was unsuccessful. Error: ${ERROR_MESSAGE}")

    endif()

    if (NOT EXISTS ${DXC_PKG_PATH})

        message(WARNING "Could not download DirectX 12 DXC nuget package. Project will load system DXC instead.")

    endif()

endif()

if (EXISTS ${DXC_PKG_PATH})

    message(STATUS "Extracting DXC nuget package contents!")

    # Convert the file from .nupkg to .zip to extract its contents
    configure_file(${DXC_PKG_PATH} ${DXC_ZIP_PATH} COPYONLY)

    file(MAKE_DIRECTORY  ${DXC_EXTRACT_FOLDER})

    # Extract contents from .zip file to created directory
    execute_process(
        COMMAND ${CMAKE_COMMAND} "-E" "tar" "xvz" "${DXC_ZIP_PATH}"
        WORKING_DIRECTORY ${DXC_EXTRACT_FOLDER}
    )

    set(DXC_PATH "${DXC_EXTRACT_FOLDER}/build/native/bin/x64")

    set(DXC_FOUND TRUE)

endif()