# Find StellarSolver.
#
# Defines:
#   STELLARSOLVER_FOUND
#   STELLARSOLVER_INCLUDE_DIRS
#   STELLARSOLVER_LINK_LIBS
#   STELLARSOLVER_LINK_DIRS
#   STELLARSOLVER_LINK_OPTIONS
#
# Avoid pkg-config IMPORTED_TARGET mode because some distro .pc files advertise
# stale include directories, which CMake validates before the project can filter
# them.

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(STELLARSOLVER_PC QUIET stellarsolver)
endif()

set(_STELLARSOLVER_INCLUDE_HINTS
    ${STELLARSOLVER_PC_INCLUDE_DIRS}
    ${STELLARSOLVER_PC_INCLUDEDIR}
    ${STELLARSOLVER_DIR}/include
    ${STELLARSOLVER_DIR}/include/libstellarsolver
    /usr/include
    /usr/local/include
    /opt/homebrew/include
    "C:/Program Files/StellarSolver/include"
    "$ENV{PROGRAMFILES}/StellarSolver/include"
    "C:/msys64/mingw64/include"
    "C:/msys64/ucrt64/include"
    "$ENV{MINGW_PREFIX}/include"
)

find_path(STELLARSOLVER_INCLUDE_DIR
    NAMES stellarsolver.h
    HINTS ${_STELLARSOLVER_INCLUDE_HINTS}
    PATH_SUFFIXES libstellarsolver
)

if(STELLARSOLVER_PC_FOUND)
    set(STELLARSOLVER_INCLUDE_DIRS ${STELLARSOLVER_PC_INCLUDE_DIRS})
    list(FILTER STELLARSOLVER_INCLUDE_DIRS INCLUDE REGEX ".+")
    foreach(_include_dir IN LISTS STELLARSOLVER_PC_INCLUDE_DIRS)
        if(NOT EXISTS "${_include_dir}")
            list(REMOVE_ITEM STELLARSOLVER_INCLUDE_DIRS "${_include_dir}")
            message(WARNING "Ignoring non-existent StellarSolver include directory from pkg-config: ${_include_dir}")
        endif()
    endforeach()

    if(STELLARSOLVER_INCLUDE_DIR AND NOT STELLARSOLVER_INCLUDE_DIR IN_LIST STELLARSOLVER_INCLUDE_DIRS)
        list(APPEND STELLARSOLVER_INCLUDE_DIRS ${STELLARSOLVER_INCLUDE_DIR})
    endif()

    set(STELLARSOLVER_LINK_DIRS ${STELLARSOLVER_PC_LIBRARY_DIRS})
    set(STELLARSOLVER_LINK_OPTIONS ${STELLARSOLVER_PC_LDFLAGS_OTHER})
    set(STELLARSOLVER_LINK_LIBS ${STELLARSOLVER_PC_LINK_LIBRARIES})
    if(NOT STELLARSOLVER_LINK_LIBS)
        set(STELLARSOLVER_LINK_LIBS ${STELLARSOLVER_PC_LIBRARIES})
    endif()
else()
    find_library(STELLARSOLVER_LIBRARY
        NAMES stellarsolver
        HINTS
            ${STELLARSOLVER_DIR}/lib
            /usr/local/lib
            /opt/homebrew/lib
            "C:/Program Files/StellarSolver/lib"
            "$ENV{PROGRAMFILES}/StellarSolver/lib"
            "C:/msys64/mingw64/lib"
            "C:/msys64/ucrt64/lib"
            "$ENV{MINGW_PREFIX}/lib"
    )

    set(STELLARSOLVER_INCLUDE_DIRS ${STELLARSOLVER_INCLUDE_DIR})
    set(STELLARSOLVER_LINK_LIBS ${STELLARSOLVER_LIBRARY})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(StellarSolver
    REQUIRED_VARS STELLARSOLVER_INCLUDE_DIRS STELLARSOLVER_LINK_LIBS
)

if(STELLARSOLVER_FOUND)
    message(STATUS "StellarSolver include dirs: ${STELLARSOLVER_INCLUDE_DIRS}")
    message(STATUS "StellarSolver link libs: ${STELLARSOLVER_LINK_LIBS}")
endif()
