set(DX12_AGILITY_SDK_NUPKG_NAME D3D12AgilitySdk.nupkg)
set(DX12_AGILITY_SDK_ZIP_NAME D3D12AgilitySdk.zip)
set(AGILITY_SDK_SCRATCH_FOLDER "${CMAKE_CURRENT_BINARY_DIR}")
set(AGILITY_SDK_PKG_PATH "${AGILITY_SDK_SCRATCH_FOLDER}/${DX12_AGILITY_SDK_NUPKG_NAME}")
set(AGILITY_SDK_ZIP_PATH "${AGILITY_SDK_SCRATCH_FOLDER}/${DX12_AGILITY_SDK_ZIP_NAME}")
set(AGILITY_SDK_EXTRACT_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/extern/AgilitySDK")
set(AGILITY_SDK_PATH ${AGILITY_SDK_EXTRACT_FOLDER} CACHE STRING "Agility SDK Path")
set(AGILITY_SDK_FOUND FALSE)

if (NOT EXISTS ${AGILITY_SDK_PKG_PATH})

    if (INTERNET_CONNECTION_AVAILABLE)

        message(INFO "Downloading DirectX 12 Agility SDK nuget package...")

        file(DOWNLOAD
            https://www.nuget.org/api/v2/package/Microsoft.Direct3D.D3D12${AGILITY_SDK_DOWNLOAD_POSTFIX}
            ${AGILITY_SDK_PKG_PATH}
            STATUS DOWNLOAD_STATUS
        )

        list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
        list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)

        if (${STATUS_CODE} EQUAL 0)

            message(STATUS "Agility SDK download complete!")

        else()

            message(WARNING "Download was unsuccessful. Error: ${ERROR_MESSAGE}")

        endif()

        if (NOT EXISTS ${AGILITY_SDK_PKG_PATH})

            message(WARNING "Could not download DirectX 12 Agility SDK nuget package. Project will load system D3D12Core.dll instead.")

        endif()

    else()

        message(WARNING "No internet connection. Cannot download DirectX 12 Agility SDK nuget package. Project will load system D3D12Core.dll instead.")

    endif()

endif()

if (EXISTS ${AGILITY_SDK_PKG_PATH})

    message(STATUS "Extracting Agility SDK nuget package contents!")

    # Convert the file from .nupkg to .zip to extract its contents
    configure_file(${AGILITY_SDK_PKG_PATH} ${AGILITY_SDK_ZIP_PATH} COPYONLY)

    file(MAKE_DIRECTORY  ${AGILITY_SDK_EXTRACT_FOLDER})

    # Extract contents from .zip file to created directory
    execute_process(
        COMMAND ${CMAKE_COMMAND} "-E" "tar" "xvz" "${AGILITY_SDK_ZIP_PATH}"
        WORKING_DIRECTORY ${AGILITY_SDK_EXTRACT_FOLDER}
    )

    if (WINPIX_EVENT_RUNTIME_ARM64)

        set(AGILITY_SDK_PATH "${AGILITY_SDK_EXTRACT_FOLDER}/build/native/bin/arm64")

    else()

        set(AGILITY_SDK_PATH "${AGILITY_SDK_EXTRACT_FOLDER}/build/native/bin/x64")

    endif()

    set(AGILITY_SDK_FOUND TRUE)

endif()