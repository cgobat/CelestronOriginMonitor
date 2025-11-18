######################################################################
# Celestron Origin Monitor .pro file for macOS/XCode
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

# macOS specific settings
CONFIG += app_bundle
QMAKE_MACOSX_DEPLOYMENT_TARGET = 13.0  # Updated to match your existing setting
QMAKE_INFO_PLIST = Info.plist
QMAKE_LFLAGS += -Wl,--allow-multiple-definition

# Include path
INCLUDEPATH += .
LIBS += -lnova
INCLUDEPATH += /opt/homebrew/include ../naif/cspice/include

# CSPICE (adjust paths to where you extracted it)
INCLUDEPATH += /path/to/cspice/include
LIBS += ../naif/cspice/lib/cspice.a

# Enable modern C++ features
CONFIG += c++17

# Qt modules
QT += core widgets network websockets

# Original source files
SOURCES += \
    main.cpp \
    TelescopeDataProcessor.cpp \
    TelescopeGUI.cpp \
    CoordinateUtils.cpp \
    TelescopeDataProcessor.cpp \
    TelescopeGUI.cpp \
    OriginBackend.cpp \

# Original header files
HEADERS += \
    TelescopeData.hpp \
    TelescopeDataProcessor.hpp \
    TelescopeGUI.hpp \
    CoordinateUtils.hpp \
    MessierCatalog.hpp \
    TelescopeData.hpp \
    TelescopeDataProcessor.hpp \
    TelescopeGUI.hpp \
    OriginBackend.hpp \

# Default rules
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# macOS icon (comment out if app.icns doesn't exist)
# ICON = app.icns

# Compiler and linker flags for macOS
macx {
    QMAKE_CXXFLAGS += -Werror=return-type
    QMAKE_LFLAGS += -Wl,-rpath,@executable_path/../Frameworks
    
    # For XCode build
    CONFIG += debug_and_release build_all relative_qt_rpath
    
    # Bundle identifier
    QMAKE_TARGET_BUNDLE_PREFIX = com.yourdomain
    QMAKE_BUNDLE = CelestronOriginMonitor
    
    # Deployment target
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 13.0
    QMAKE_LFLAGS += -Wl,-rpath,@executable_path/../Frameworks
}

# Post-build deployment
macx {
    # Use QMAKE_BUNDLE_DATA instead of QMAKE_POST_LINK for Info.plist modifications
    QMAKE_INFO_PLIST = Info.plist
    
    # This creates a script that will run after the app bundle is created
    deploy.commands = /usr/libexec/PlistBuddy -c \"Set :CFBundleShortVersionString $$VERSION\" $$DESTDIR/$${TARGET}.app/Contents/Info.plist
    
    # Make sure this runs after the target is built
    deploy.depends = $(TARGET)
    
    # Add the deploy step to your build process
    QMAKE_EXTRA_TARGETS += deploy
    POST_TARGETDEPS += deploy
}
