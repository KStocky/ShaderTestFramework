include_guard(GLOBAL)

include("${CMAKE_HOME_DIRECTORY}/cmake/Nuget.cmake")

function(nuget_get_agility_sdk OUT_SUCCEEDED OUT_INCLUDE_PATH OUT_BINARY_PATH OUT_SDK_VER)

    set(MAJOR_VER "1")
    set(MINOR_VER "610")
    set(PATCH_VER "5")

    nuget_pkg_get("Microsoft.Direct3D.D3D12" "${MAJOR_VER}.${MINOR_VER}.${PATCH_VER}" ${OUT_SUCCEEDED} PKG_PATH)

    if (${${OUT_SUCCEEDED}})
        set(${OUT_INCLUDE_PATH} "${PKG_PATH}/build/native/include")
        set(${OUT_BINARY_PATH} "${PKG_PATH}/build/native/bin/x64")
        set(${OUT_SDK_VER} "${MINOR_VER}")
    endif()

    return(PROPAGATE ${OUT_SUCCEEDED} ${OUT_INCLUDE_PATH} ${OUT_BINARY_PATH} ${OUT_SDK_VER})
endfunction()

function(nuget_get_dxc OUT_SUCCEEDED OUT_INCLUDE_PATH OUT_BINARY_PATH OUT_LIB_PATH)

    nuget_pkg_get("Microsoft.Direct3D.DXC" "1.7.2308.12" ${OUT_SUCCEEDED} PKG_PATH)

    if (${${OUT_SUCCEEDED}})
        set(${OUT_INCLUDE_PATH} "${PKG_PATH}/build/native/include")
        set(${OUT_BINARY_PATH} "${PKG_PATH}/build/native/bin/x64")
        set(${OUT_LIB_PATH} "${PKG_PATH}/build/native/lib/x64")
    endif()

    return(PROPAGATE ${OUT_SUCCEEDED} ${OUT_INCLUDE_PATH} ${OUT_BINARY_PATH} ${OUT_LIB_PATH})
endfunction()

function(nuget_get_warp OUT_SUCCEEDED OUT_BINARY_PATH)

    nuget_pkg_get("Microsoft.Direct3D.WARP" "1.0.7.1" ${OUT_SUCCEEDED} PKG_PATH)

    if (${${OUT_SUCCEEDED}})
        set(${OUT_BINARY_PATH} "${PKG_PATH}/build/native/amd64")
    endif()

    return(PROPAGATE ${OUT_SUCCEEDED} ${OUT_BINARY_PATH})
endfunction()
