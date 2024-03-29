include_guard(GLOBAL)

include(Nuget)

function(nuget_get_agility_sdk IN_TARGET OUT_SUCCEEDED OUT_INCLUDE_PATH OUT_BINARY_PATH OUT_SDK_VER)

    set(MAJOR_VER "1")
    set(MINOR_VER "613")
    set(PATCH_VER "1")

    nuget_pkg_get(${IN_TARGET} "Microsoft.Direct3D.D3D12" "${MAJOR_VER}.${MINOR_VER}.${PATCH_VER}" ${OUT_SUCCEEDED} PKG_PATH)

    if (${${OUT_SUCCEEDED}})
        set(${OUT_INCLUDE_PATH} "${PKG_PATH}/build/native/include")
        set(${OUT_BINARY_PATH} "${PKG_PATH}/build/native/bin/x64")
        set(${OUT_SDK_VER} "${MINOR_VER}")
    endif()

    return(PROPAGATE ${OUT_SUCCEEDED} ${OUT_INCLUDE_PATH} ${OUT_BINARY_PATH} ${OUT_SDK_VER})
endfunction()

function(nuget_get_dxc IN_TARGET OUT_SUCCEEDED OUT_INCLUDE_PATH OUT_BINARY_PATH OUT_LIB_PATH)

    nuget_pkg_get(${IN_TARGET} "Microsoft.Direct3D.DXC" "1.7.2308.12" ${OUT_SUCCEEDED} PKG_PATH)

    if (${${OUT_SUCCEEDED}})
        set(${OUT_INCLUDE_PATH} "${PKG_PATH}/build/native/include")
        set(${OUT_BINARY_PATH} "${PKG_PATH}/build/native/bin/x64")
        set(${OUT_LIB_PATH} "${PKG_PATH}/build/native/lib/x64")
    endif()

    return(PROPAGATE ${OUT_SUCCEEDED} ${OUT_INCLUDE_PATH} ${OUT_BINARY_PATH} ${OUT_LIB_PATH})
endfunction()

function(nuget_get_warp IN_TARGET OUT_SUCCEEDED OUT_BINARY_PATH)

    nuget_pkg_get(${IN_TARGET} "Microsoft.Direct3D.WARP" "1.0.11" ${OUT_SUCCEEDED} PKG_PATH)

    if (${${OUT_SUCCEEDED}})
        set(${OUT_BINARY_PATH} "${PKG_PATH}/build/native/amd64")
    endif()

    return(PROPAGATE ${OUT_SUCCEEDED} ${OUT_BINARY_PATH})
endfunction()

function(nuget_get_WinPixEventRuntime IN_TARGET OUT_SUCCEEDED OUT_INCLUDE_PATH OUT_BINARY_PATH)

    nuget_pkg_get(${IN_TARGET} "WinPixEventRuntime" "1.0.240308001" ${OUT_SUCCEEDED} PKG_PATH)

    if (${${OUT_SUCCEEDED}})
        set(${OUT_INCLUDE_PATH} "${PKG_PATH}/Include")
        set(${OUT_BINARY_PATH} "${PKG_PATH}/bin/x64")
    endif()

    return(PROPAGATE ${OUT_SUCCEEDED} ${OUT_INCLUDE_PATH} ${OUT_BINARY_PATH})
endfunction()
