cmake_minimum_required(VERSION 3.16)

project(CelestronOriginMonitor VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# On MSVC, ensure M_PI and friends are defined
if(MSVC)
    add_compile_definitions(_USE_MATH_DEFINES)
endif()

# Find Qt packages. Qt 6 is preferred, but Debian/Ubuntu's packaged
# StellarSolver is currently built against Qt 5, so allow either Qt major.
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Network WebSockets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Network WebSockets)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(StellarSolver REQUIRED)

# Main GUI application sources
set(SOURCES
    main.cpp
    CoordinateUtils.cpp
    TelescopeDataProcessor.cpp
    TelescopeGUI.cpp
    OriginBackend.cpp
    LogReplayDialog.cpp
    CommandInterface.cpp
    ImageAnalyzer.cpp
    AutopilotController.cpp
    AutopilotTab.cpp
    JsonMonitorTab.cpp
    PlateSolver.cpp
    MountModel.cpp
    AlignmentController.cpp
    AlignmentTab.cpp
)

set(HEADERS
    CoordinateUtils.hpp
    MessierCatalog.hpp
    TelescopeData.hpp
    TelescopeDataProcessor.hpp
    TelescopeGUI.hpp
    OriginBackend.hpp
    LogReplayDialog.hpp
    CommandInterface.hpp
    ImageAnalyzer.hpp
    AutopilotController.hpp
    AutopilotTab.hpp
    JsonMonitorTab.hpp
    PlateSolver.hpp
    MountModel.hpp
    AlignmentController.hpp
    AlignmentTab.hpp
)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${STELLARSOLVER_INCLUDE_DIRS}
)

if(STELLARSOLVER_LINK_DIRS)
    target_link_directories(${PROJECT_NAME} PRIVATE ${STELLARSOLVER_LINK_DIRS})
endif()

if(STELLARSOLVER_LINK_OPTIONS)
    target_link_options(${PROJECT_NAME} PRIVATE ${STELLARSOLVER_LINK_OPTIONS})
endif()

# StellarSolver on Windows is built as a static library; suppress dllimport/dllexport
# by force-including a header that defines STELLARSOLVER_API to nothing before
# stellarsolver_export.h can set it to dllimport
if(WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -include ${CMAKE_CURRENT_SOURCE_DIR}/stellarsolver_static.h)
endif()

# StellarSolver transitive dependencies (static linking on Windows)
# Must come before Qt in link order since stellarsolver references Qt symbols
if(WIN32)
    find_package(PkgConfig)
    pkg_check_modules(CFITSIO cfitsio)
    pkg_check_modules(GSL gsl)
    find_library(WCSLIB_LIBRARY NAMES wcs HINTS ${STELLARSOLVER_DIR}/lib)
    find_library(MMAN_LIBRARY NAMES mman HINTS ${STELLARSOLVER_DIR}/lib)
    set(STELLARSOLVER_TRANSITIVE_LIBS
        ${CFITSIO_LIBRARIES} ${GSL_LIBRARIES}
        ${WCSLIB_LIBRARY} ${MMAN_LIBRARY})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${STELLARSOLVER_LINK_LIBS}
    ${STELLARSOLVER_TRANSITIVE_LIBS}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::WebSockets
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION .
)

# On Windows, copy Qt DLLs to output directory for easy running
if(WIN32)
    if(QT_VERSION_MAJOR EQUAL 6)
        set(QT_BIN_HINT "${Qt6_DIR}/../../../bin")
    else()
        set(QT_BIN_HINT "${Qt5_DIR}/../../../bin")
    endif()

    # Use windeployqt to gather Qt DLLs
    find_program(WINDEPLOYQT windeployqt
        HINTS
            ${QT_BIN_HINT}
            "$ENV{MINGW_PREFIX}/bin"
            "C:/msys64/mingw64/bin"
            "C:/msys64/ucrt64/bin"
    )
    if(WINDEPLOYQT)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${WINDEPLOYQT} --no-translations $<TARGET_FILE:${PROJECT_NAME}>
            COMMENT "Running windeployqt..."
        )
    endif()
endif()

option(ORIGINMONITOR_PACKAGE_DEB "Enable Debian package generation" OFF)
if(ORIGINMONITOR_PACKAGE_DEB)
    include(PackageDeb)
endif()
