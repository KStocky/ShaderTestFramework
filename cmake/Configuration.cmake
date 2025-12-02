include_guard(GLOBAL)

function(addDynamicDebuggingConfig)
    
    if(NOT (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC))
        return()
    endif()

    if(MSVC_VERSION VERSION_LESS 1944)
        return()
    endif()

    if(NOT CMAKE_CONFIGURATION_TYPES)
        return()
    endif()

    list(APPEND CMAKE_CONFIGURATION_TYPES DynDeopt)
    list(REMOVE_DUPLICATES CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
    "Modified to include dynamic deopt"
    FORCE)

    set(CMAKE_CXX_FLAGS_DYNDEOPT "${CMAKE_CXX_FLAGS_RELEASE} /dynamicdeopt /Zi")
    set(CMAKE_CXX_FLAGS_DYNDEOPT ${CMAKE_CXX_FLAGS_DYNDEOPT} CACHE STRING "CXX flags for DynDeopt configuration" FORCE)

    set(CMAKE_EXE_LINKER_FLAGS_DYNDEOPT "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /dynamicdeopt /debug")
    set(CMAKE_EXE_LINKER_FLAGS_DYNDEOPT ${CMAKE_EXE_LINKER_FLAGS_DYNDEOPT} CACHE STRING "Linker flags for DynDeopt configuration" FORCE)

    set(CMAKE_MODULE_LINKER_FLAGS_DYNDEOPT "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /dynamicdeopt /debug")
    set(CMAKE_MODULE_LINKER_FLAGS_DYNDEOPT ${CMAKE_MODULE_LINKER_FLAGS_DYNDEOPT} CACHE STRING "Flags used by the linker during the creation of modules during for DynDeopt configuration" FORCE)
            
    set(CMAKE_SHARED_LINKER_FLAGS_DYNDEOPT "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /dynamicdeopt /debug")
    set(CMAKE_SHARED_LINKER_FLAGS_DYNDEOPT ${CMAKE_SHARED_LINKER_FLAGS_DYNDEOPT} CACHE STRING "Flags used by the linker during the creation of shared libraries during DYNDEOPT builds." FORCE)

    set(CMAKE_STATIC_LINKER_FLAGS_DYNDEOPT "${CMAKE_STATIC_LINKER_FLAGS_RELEASE} /dynamicdeopt")
    set(CMAKE_STATIC_LINKER_FLAGS_DYNDEOPT ${CMAKE_STATIC_LINKER_FLAGS_DYNDEOPT} CACHE STRING "Flags used by the linker during the creation of static libraries during DYNDEOPT builds." FORCE)

    mark_as_advanced(
        CMAKE_CXX_FLAGS_DYNDEOPT
        CMAKE_EXE_LINKER_FLAGS_DYNDEOPT
        CMAKE_MODULE_LINKER_FLAGS_DYNDEOPT
        CMAKE_SHARED_LINKER_FLAGS_DYNDEOPT
        CMAKE_STATIC_LINKER_FLAGS_DYNDEOPT)

endfunction()