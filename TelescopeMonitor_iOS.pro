# TelescopeMonitor_iOS.pro
# Fixed version with renamed resource folder

QT += core gui widgets network websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TelescopeMonitor
TEMPLATE = app

CONFIG += c++17

# iOS specific configuration
ios {
    # Minimum iOS version
    QMAKE_IOS_DEPLOYMENT_TARGET = 13.0
    
    # Bundle identifier - CHANGE THIS to your own!
    QMAKE_TARGET_BUNDLE_PREFIX = com.astronomy
    
    # Device family: 1=iPhone, 2=iPad, 1,2=Universal
    QMAKE_APPLE_TARGETED_DEVICE_FAMILY = 1,2
    
    # ==========================================
    # CRITICAL FIX FOR INFO.PLIST
    # ==========================================
    
    # Info.plist at project root
    QMAKE_INFO_PLIST = $$PWD/Info.plist
    
    # Also set it for Xcode build system
    info_plist.name = INFOPLIST_FILE
    info_plist.value = $$PWD/Info.plist
    QMAKE_MAC_XCODE_SETTINGS += info_plist
    
    # ==========================================
    # FOLDER RENAMED: ios -> ios_resources
    # This prevents conflict with <ios> C++ header
    # ==========================================
    
    # Launch Screen
    exists($$PWD/ios_resources/LaunchScreen.storyboard) {
        QMAKE_IOS_LAUNCH_SCREEN = $$PWD/ios_resources/LaunchScreen.storyboard
    }
    
    # Assets catalog
    exists($$PWD/ios_resources/Assets.xcassets) {
        QMAKE_ASSET_CATALOGS += $$PWD/ios_resources/Assets.xcassets
    }
    
    # Bundle SPICE kernel data files (if they exist)
    exists($$PWD/spice_kernels) {
        spice_kernels.files = $$files($$PWD/spice_kernels/*.bsp) \
                              $$files($$PWD/spice_kernels/*.tls)
        spice_kernels.path = spice_kernels
        QMAKE_BUNDLE_DATA += spice_kernels
    }
    
    # Enable ARC
    QMAKE_CXXFLAGS += -fobjc-arc
    
    # Fix header search paths
    QMAKE_CXXFLAGS += -Wno-unknown-pragmas
    
    # Enable automatic signing
    automatic_signing.name = "CODE_SIGN_STYLE"
    automatic_signing.value = "Automatic"
    QMAKE_MAC_XCODE_SETTINGS += automatic_signing
    
    # Silence SDK version warning
    CONFIG += sdk_no_version_check
}

# macOS specific
macx:!ios {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15
}

# Don't define Q_OS_IOS - Qt already defines it
# Removes the macro redefinition warning
ios {
    # Q_OS_IOS is automatically defined by Qt
}

# Core application files
SOURCES += \
    main.cpp \
    TelescopeGUI.cpp \
    OriginBackend.cpp \
    TelescopeDataProcessor.cpp \
    CommandInterface.cpp

HEADERS += \
    TelescopeGUI.hpp \
    OriginBackend.hpp \
    TelescopeData.hpp \
    TelescopeDataProcessor.hpp \
    CommandInterface.hpp

# Optional files
exists($$PWD/LogReplayDialog.cpp) {
    SOURCES += LogReplayDialog.cpp
    HEADERS += LogReplayDialog.hpp
}

exists($$PWD/CoordinateUtils.cpp) {
    SOURCES += CoordinateUtils.cpp
    HEADERS += CoordinateUtils.hpp
}

exists($$PWD/PlatformUtils.hpp) {
    HEADERS += PlatformUtils.hpp
}

# Include paths
INCLUDEPATH += $$PWD

# Deployment settings
ios {
    CONFIG(device, device|simulator) {
        QMAKE_APPLE_DEVICE_ARCHS = arm64
    }
    CONFIG(simulator, device|simulator) {
        QMAKE_APPLE_DEVICE_ARCHS = arm64 x86_64
    }
}

# Compiler settings
QMAKE_CXXFLAGS += -Wall -Wextra

CONFIG(debug, debug|release) {
    DEFINES += DEBUG_BUILD
}

CONFIG(release, debug|release) {
    DEFINES += RELEASE_BUILD
    QMAKE_CXXFLAGS += -O2
}
