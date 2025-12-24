# TelescopeMonitor_iOS.pro - With App Icons Support

QT += core gui widgets network websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TelescopeMonitor
TEMPLATE = app

CONFIG += c++17

# iOS specific configuration
ios {
    # Minimum iOS version
    QMAKE_IOS_DEPLOYMENT_TARGET = 16.0
    
    # Bundle identifier - CHANGE THIS to your own!
    QMAKE_TARGET_BUNDLE_PREFIX = uk.kimmitt
    
    # Device family: 1=iPhone, 2=iPad, 1,2=Universal
    QMAKE_APPLE_TARGETED_DEVICE_FAMILY = 1,2
    
    # ==========================================
    # INFO.PLIST CONFIGURATION
    # ==========================================
    
    # Info.plist at project root
    QMAKE_INFO_PLIST = $$PWD/Info.plist
    
    # Pass to Xcode build system
    info_plist.name = INFOPLIST_FILE
    info_plist.value = $$PWD/Info.plist
    QMAKE_MAC_XCODE_SETTINGS += info_plist
    
    # ==========================================
    # APP ICONS (Asset Catalog)
    # ==========================================
    
    # Reference the AppIcon asset catalog
    QMAKE_ASSET_CATALOGS += ios_resources/AppIcon.appiconset
    
    # ==========================================
    # LAUNCH SCREEN
    # ==========================================
    
    # Launch Screen
    exists($$PWD/ios_resources/LaunchScreen.storyboard) {
        QMAKE_IOS_LAUNCH_SCREEN = $$PWD/ios_resources/LaunchScreen.storyboard
    }
    
    # ==========================================
    # OTHER RESOURCES
    # ==========================================
    
    # Bundle SPICE kernel data files (if they exist)
    exists($$PWD/spice_kernels) {
        spice_kernels.files = $$files($$PWD/spice_kernels/*.bsp) \
                              $$files($$PWD/spice_kernels/*.tls)
        spice_kernels.path = spice_kernels
        QMAKE_BUNDLE_DATA += spice_kernels
    }
    
    # ==========================================
    # COMPILER FLAGS
    # ==========================================
    
    # Enable ARC
    QMAKE_CXXFLAGS += -fobjc-arc
    
    # Fix warnings
    QMAKE_CXXFLAGS += -Wno-unknown-pragmas
    
    # ==========================================
    # XCODE BUILD SETTINGS
    # ==========================================
    
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
