######################################################################
# Celestron Origin Monitor .pro file - Cross-platform
######################################################################

# Project configuration
VERSION = 1.0.0
TEMPLATE = app
TARGET = CelestronOriginMonitor
DESTDIR = build
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui

# Include path
INCLUDEPATH += .

# Enable modern C++ features
CONFIG += c++17
QMAKE_CXXFLAGS += -g

# Qt modules
QT += core widgets network websockets

# Original source files
SOURCES += \
    main.cpp \
    CoordinateUtils.cpp \
    TelescopeDataProcessor.cpp \
    TelescopeGUI.cpp \
    OriginBackend.cpp \
    LogReplayDialog.cpp \
    CommandInterface.cpp \
    ImageAnalyzer.cpp \
    AutopilotController.cpp \
    AutopilotTab.cpp \
    JsonMonitorTab.cpp \
    PlateSolver.cpp \
    MountModel.cpp \
    AlignmentController.cpp \
    AlignmentTab.cpp \

# Original header files
HEADERS += \
    CoordinateUtils.hpp \
    MessierCatalog.hpp \
    TelescopeData.hpp \
    TelescopeDataProcessor.hpp \
    TelescopeGUI.hpp \
    OriginBackend.hpp \
    LogReplayDialog.hpp \
    CommandInterface.hpp \
    ImageAnalyzer.hpp \
    AutopilotController.hpp \
    AutopilotTab.hpp \
    JsonMonitorTab.hpp \
    PlateSolver.hpp \
    MountModel.hpp \
    AlignmentController.hpp \
    AlignmentTab.hpp \

# Platform-specific configuration
macx {
    CONFIG += app_bundle sdk_no_version_check
    QMAKE_INFO_PLIST = Info.plist

    INCLUDEPATH += /opt/homebrew/include
    INCLUDEPATH += /usr/local/include/libstellarsolver
    LIBS += -L/usr/local/lib -lstellarsolver

    QMAKE_CXXFLAGS += -Werror=return-type
    QMAKE_LFLAGS += -Wl,-rpath,@executable_path/../Frameworks
    QMAKE_LFLAGS += -Wl,-rpath,/usr/local/lib

    CONFIG += build_all relative_qt_rpath
    QMAKE_TARGET_BUNDLE_PREFIX = com.yourdomain
    QMAKE_BUNDLE = CelestronOriginMonitor
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 15.0

    # Post-build deployment
    QMAKE_INFO_PLIST = Info.plist
    deploy.commands = /usr/libexec/PlistBuddy -c \"Set :CFBundleShortVersionString $$VERSION\" $$DESTDIR/$${TARGET}.app/Contents/Info.plist
    deploy.depends = $(TARGET)
    QMAKE_EXTRA_TARGETS += deploy
    POST_TARGETDEPS += deploy
}

win32 {
    # MSVC needs _USE_MATH_DEFINES for M_PI; MinGW provides it by default
    msvc: DEFINES += _USE_MATH_DEFINES

    # StellarSolver - check env var, then MSYS2/MinGW64 paths, then fallback
    STELLARSOLVER_DIR = $$(STELLARSOLVER_DIR)
    isEmpty(STELLARSOLVER_DIR) {
        # Try MSYS2/MinGW64 standard paths
        exists(C:/msys64/mingw64/include/libstellarsolver/stellarsolver.h) {
            STELLARSOLVER_DIR = C:/msys64/mingw64
        } else {
            STELLARSOLVER_DIR = "C:/Program Files/StellarSolver"
        }
    }
    INCLUDEPATH += $$STELLARSOLVER_DIR/include
    INCLUDEPATH += $$STELLARSOLVER_DIR/include/libstellarsolver
    LIBS += -L$$STELLARSOLVER_DIR/lib -lstellarsolver
}

unix:!macx {
    INCLUDEPATH += /usr/include/libstellarsolver
    INCLUDEPATH += /usr/local/include/libstellarsolver

    CONFIG += link_pkgconfig
    packagesExist(stellarsolver) {
        PKGCONFIG += stellarsolver
    } else {
        LIBS += -L/usr/local/lib -lstellarsolver
    }
}

# Default install rules
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
