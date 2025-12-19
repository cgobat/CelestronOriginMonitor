# Qt iOS Porting Guide for Celestron Origin Monitor

## Overview
This guide covers porting your Qt/C++ telescope monitoring application from macOS to iOS. The app uses Qt Widgets, WebSockets, networking, and file I/O.

## Prerequisites

### 1. Development Environment
- **Xcode** (latest version from Mac App Store)
- **Qt for iOS** (Qt 6.5+ recommended)
  - Install via Qt Online Installer
  - Ensure iOS component is selected during installation
- **Apple Developer Account** (for device testing and distribution)
- **iOS Device** running iOS 12.0 or later (recommended iOS 15+)

### 2. Qt Installation for iOS
```bash
# Verify Qt installation includes iOS
ls ~/Qt/6.x.x/ios/
# Should show: bin/ lib/ include/ mkspecs/ plugins/ qml/
```

## Key Changes Required

### 1. Project Configuration (.pro file changes)

Create a new `.pro` file or modify existing one:

```qmake
# TelescopeMonitor_iOS.pro

QT += core gui widgets network websockets

TARGET = TelescopeMonitor
TEMPLATE = app

# iOS specific configuration
ios {
    # Minimum iOS version
    QMAKE_IOS_DEPLOYMENT_TARGET = 12.0
    
    # Bundle identifier (change to your own)
    QMAKE_TARGET_BUNDLE_PREFIX = com.yourcompany
    
    # Device family: 1=iPhone, 2=iPad, 1,2=Universal
    QMAKE_APPLE_TARGETED_DEVICE_FAMILY = 1,2
    
    # Required capabilities
    QMAKE_INFO_PLIST = ios/Info.plist
    
    # Disable bitcode if needed (Qt 6 usually doesn't need this)
    # QMAKE_CXXFLAGS += -fembed-bitcode
    
    # Icon files
    ios_icon.files = $$files($$PWD/ios/AppIcon*.png)
    QMAKE_BUNDLE_DATA += ios_icon
    
    # Launch screen
    app_launch_images.files = $$PWD/ios/LaunchScreen.storyboard
    QMAKE_BUNDLE_DATA += app_launch_images
}

# Source files
SOURCES += \
    main.cpp \
    TelescopeGUI.cpp \
    OriginBackend.cpp \
    TelescopeDataProcessor.cpp \
    CommandInterface.cpp \
    CometTracker.cpp \
    CometObserver.cpp \
    CoordinateUtils.cpp \
    LogReplayDialog.cpp

HEADERS += \
    TelescopeGUI.hpp \
    OriginBackend.hpp \
    TelescopeData.hpp \
    TelescopeDataProcessor.hpp \
    CommandInterface.hpp \
    CometTracker.hpp \
    CometObserver.hpp \
    CoordinateUtils.hpp \
    LogReplayDialog.hpp

# Include any additional libraries
# LIBS += -lYourLibrary
```

### 2. Info.plist Configuration

Create `ios/Info.plist`:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>en</string>
    <key>CFBundleDisplayName</key>
    <string>Telescope Monitor</string>
    <key>CFBundleExecutable</key>
    <string>${EXECUTABLE_NAME}</string>
    <key>CFBundleIdentifier</key>
    <string>$(PRODUCT_BUNDLE_IDENTIFIER)</string>
    <key>CFBundleName</key>
    <string>${PRODUCT_NAME}</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0</string>
    <key>CFBundleVersion</key>
    <string>1</string>
    <key>LSRequiresIPhoneOS</key>
    <true/>
    
    <!-- Network permissions -->
    <key>NSAppTransportSecurity</key>
    <dict>
        <key>NSAllowsArbitraryLoads</key>
        <true/>
        <!-- Or more restrictive: -->
        <key>NSExceptionDomains</key>
        <dict>
            <key>local</key>
            <dict>
                <key>NSIncludesSubdomains</key>
                <true/>
                <key>NSTemporaryExceptionAllowsInsecureHTTPLoads</key>
                <true/>
            </dict>
        </dict>
    </dict>
    
    <!-- Local Network permission (iOS 14+) -->
    <key>NSLocalNetworkUsageDescription</key>
    <string>This app needs to discover and connect to telescopes on your local network</string>
    
    <!-- Bonjour services (for UDP broadcast discovery) -->
    <key>NSBonjourServices</key>
    <array>
        <string>_telescope._udp</string>
    </array>
    
    <!-- Location permission if needed -->
    <key>NSLocationWhenInUseUsageDescription</key>
    <string>Location is used to calculate telescope coordinates</string>
    
    <!-- Photo library (if saving images) -->
    <key>NSPhotoLibraryAddUsageDescription</key>
    <string>Save telescope images to your photo library</string>
    
    <!-- File access -->
    <key>UIFileSharingEnabled</key>
    <true/>
    <key>LSSupportsOpeningDocumentsInPlace</key>
    <true/>
    
    <!-- Supported interface orientations -->
    <key>UISupportedInterfaceOrientations</key>
    <array>
        <string>UIInterfaceOrientationPortrait</string>
        <string>UIInterfaceOrientationLandscapeLeft</string>
        <string>UIInterfaceOrientationLandscapeRight</string>
    </array>
    <key>UISupportedInterfaceOrientations~ipad</key>
    <array>
        <string>UIInterfaceOrientationPortrait</string>
        <string>UIInterfaceOrientationPortraitUpsideDown</string>
        <string>UIInterfaceOrientationLandscapeLeft</string>
        <string>UIInterfaceOrientationLandscapeRight</string>
    </array>
    
    <!-- Status bar -->
    <key>UIStatusBarHidden</key>
    <false/>
    <key>UIViewControllerBasedStatusBarAppearance</key>
    <false/>
</dict>
</plist>
```

### 3. UI Adaptations for Mobile

#### TelescopeGUI.cpp Changes

Key modifications needed in `TelescopeGUI::setupUI()`:

```cpp
void TelescopeGUI::setupUI() {
    // Detect iOS platform
#ifdef Q_OS_IOS
    // Use native iOS styling
    setStyleSheet("QWidget { font-size: 14pt; }"); // Larger touch targets
    
    // Reduce window margins for mobile
    centralWidget()->layout()->setContentsMargins(5, 5, 5, 5);
    
    // Make buttons touch-friendly (minimum 44x44 points)
    const int minButtonHeight = 44;
    
#else
    // Desktop styling
    const int minButtonHeight = 30;
#endif

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout();
    
    // Connection section with touch-friendly buttons
    QGroupBox *connectionGroup = new QGroupBox("Telescope Connection");
    QVBoxLayout *connectionLayout = new QVBoxLayout();
    
    telescopeListWidget = new QListWidget();
    telescopeListWidget->setMinimumHeight(100);
    
    connectButton = new QPushButton("Connect to Selected Telescope");
    connectButton->setMinimumHeight(minButtonHeight);
    
#ifdef Q_OS_IOS
    // On iOS, use scroll areas for long content
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(tabWidget);
    mainLayout->addWidget(scrollArea);
#else
    mainLayout->addWidget(tabWidget);
#endif
    
    // Status label at bottom
    statusLabel = new QLabel("Not connected");
    statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(statusLabel);
    
    // ... rest of UI setup
}
```

#### Touch-Friendly Controls

```cpp
QWidget* TelescopeGUI::createMountTab() {
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    
#ifdef Q_OS_IOS
    // iOS: Use larger fonts and spacing
    QFont labelFont;
    labelFont.setPointSize(14);
    
    // Create scroll area for content
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(15); // More spacing for touch
    
#else
    QVBoxLayout *contentLayout = layout;
    contentLayout->setSpacing(8);
#endif
    
    // Mount status display with larger labels
    QGroupBox *statusGroup = new QGroupBox("Mount Status");
    QGridLayout *statusLayout = new QGridLayout();
    
    int row = 0;
    statusLayout->addWidget(new QLabel("Altitude:"), row, 0);
    mountAltitudeLabel = new QLabel("N/A");
#ifdef Q_OS_IOS
    mountAltitudeLabel->setFont(labelFont);
#endif
    statusLayout->addWidget(mountAltitudeLabel, row++, 1);
    
    // ... more status fields
    
#ifdef Q_OS_IOS
    contentLayout->addWidget(statusGroup);
    scrollArea->setWidget(contentWidget);
    layout->addWidget(scrollArea);
#else
    layout->addWidget(statusGroup);
#endif
    
    return widget;
}
```

### 4. File System Changes

iOS has restricted file access. Update file handling:

```cpp
// In OriginBackend.cpp or wherever files are accessed

QString getAppDataPath() {
#ifdef Q_OS_IOS
    // iOS: Use app's Documents directory
    QStringList paths = QStandardPaths::standardLocations(
        QStandardPaths::DocumentsLocation);
    if (paths.isEmpty()) {
        return QDir::homePath();
    }
    return paths.first();
#else
    // macOS/Desktop
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#endif
}

void OriginBackend::initializeLogging() {
    QString logPath = getAppDataPath() + "/telescope_log.txt";
    
    m_logFile = new QFile(logPath);
    if (!m_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Failed to open log file:" << logPath;
        delete m_logFile;
        m_logFile = nullptr;
        return;
    }
    
    m_logStream = new QTextStream(m_logFile);
    qDebug() << "Logging to:" << logPath;
}
```

### 5. Network Discovery Adaptations

UDP broadcast discovery needs iOS-specific handling:

```cpp
void TelescopeGUI::setupDiscovery() {
    udpSocket = new QUdpSocket(this);
    
#ifdef Q_OS_IOS
    // iOS requires special handling for broadcast
    // Bind to specific port
    if (!udpSocket->bind(QHostAddress::AnyIPv4, 55555, 
                         QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        qWarning() << "Failed to bind UDP socket:" << udpSocket->errorString();
        QMessageBox::warning(this, "Network Error", 
            "Could not set up telescope discovery. Please check network permissions.");
        return;
    }
    
    // iOS may need explicit permission prompt
    // This happens automatically when first using network features
#else
    // macOS binding
    udpSocket->bind(QHostAddress::AnyIPv4, 55555, 
                    QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
#endif
    
    connect(udpSocket, &QUdpSocket::readyRead, 
            this, &TelescopeGUI::processPendingDatagrams);
}
```

### 6. Menu Bar Removal

iOS doesn't support traditional menu bars:

```cpp
void TelescopeGUI::setupUI() {
#ifndef Q_OS_IOS
    // Desktop: Create menu bar
    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *fileMenu = menuBar->addMenu("&File");
    
    QAction *replayAction = new QAction("Replay Log...", this);
    connect(replayAction, &QAction::triggered, this, &TelescopeGUI::showLogReplay);
    fileMenu->addAction(replayAction);
    
    setMenuBar(menuBar);
#else
    // iOS: Add log replay as a button in the UI instead
    QPushButton *replayButton = new QPushButton("Replay Log");
    connect(replayButton, &QPushButton::clicked, this, &TelescopeGUI::showLogReplay);
    // Add replayButton to appropriate layout
#endif
}
```

### 7. SPICE Kernels Bundling

Bundle astronomical data files with your app:

```qmake
# In .pro file
ios {
    # Bundle SPICE kernel files
    spice_kernels.files = $$PWD/spice_kernels/*
    spice_kernels.path = spice_kernels
    QMAKE_BUNDLE_DATA += spice_kernels
}
```

Access bundled resources:

```cpp
QString getKernelPath() {
#ifdef Q_OS_IOS
    // iOS: Kernels are in app bundle
    QString bundlePath = QCoreApplication::applicationDirPath();
    return bundlePath + "/spice_kernels/";
#else
    // macOS: Use development path
    return "/Users/jonathan/spice_kernels/";
#endif
}

// In CometObserver initialization
QString kernelDir = getKernelPath();
observer.loadKernels(
    kernelDir + "c2025a6.bsp",
    kernelDir + "naif0012.tls",
    kernelDir + "de440s.bsp"
);
```

## Building and Deployment

### 1. Build from Qt Creator

1. Open Qt Creator
2. Open your `.pro` file
3. Select an iOS kit from the kit selector
4. Click the green "Run" button or:
   - Select "Build" → "Build Project"
   - Select "Build" → "Run" to deploy to device/simulator

### 2. Command Line Build

```bash
# Set up environment
export QTDIR=~/Qt/6.x.x/ios
export PATH=$QTDIR/bin:$PATH

# Generate Xcode project
qmake -spec macx-ios-clang CONFIG+=device CONFIG+=iphoneos TelescopeMonitor_iOS.pro

# Build
xcodebuild -project TelescopeMonitor.xcodeproj \
    -scheme TelescopeMonitor \
    -configuration Release \
    -destination 'platform=iOS,name=Your Device Name'
```

### 3. Simulator Testing

```bash
# Build for simulator
qmake -spec macx-ios-clang CONFIG+=simulator CONFIG+=iphonesimulator-clang

# Or from Qt Creator: Select iOS Simulator kit
```

### 4. Code Signing

In Xcode or via command line:

```bash
# Set your development team
xcodebuild -project TelescopeMonitor.xcodeproj \
    -scheme TelescopeMonitor \
    DEVELOPMENT_TEAM=YOUR_TEAM_ID
```

## Common Issues and Solutions

### Issue 1: Network Permissions

**Problem:** App can't discover telescopes on local network.

**Solution:** 
- Ensure `NSLocalNetworkUsageDescription` is in Info.plist
- Test on physical device (simulators have different network behavior)
- Check iOS 14+ local network permission dialog appears

### Issue 2: File Access

**Problem:** Can't read/write files.

**Solution:**
```cpp
// Always use QStandardPaths for iOS
QString dataPath = QStandardPaths::writableLocation(
    QStandardPaths::DocumentsLocation);
```

### Issue 3: UI Elements Too Small

**Problem:** Buttons and text too small for touch.

**Solution:**
```cpp
#ifdef Q_OS_IOS
// Minimum 44x44 points for touch targets (Apple HIG)
button->setMinimumSize(44, 44);
// Use larger fonts
QFont font = button->font();
font.setPointSize(16);
button->setFont(font);
#endif
```

### Issue 4: App Crashes on Launch

**Problem:** Missing permissions or resources.

**Solution:**
- Check Console app for crash logs
- Verify Info.plist has all required keys
- Ensure bundled resources are accessible
- Test in simulator first, then device

### Issue 5: WebSocket Connection Fails

**Problem:** Can't connect to telescope WebSocket.

**Solution:**
- Add appropriate NSAppTransportSecurity exceptions
- Use WSS (secure WebSocket) if possible
- Check firewall settings on telescope device

## Testing Checklist

- [ ] App launches without crashes
- [ ] UI is readable and touch-friendly
- [ ] Network discovery finds telescopes
- [ ] WebSocket connection establishes
- [ ] Can send commands to telescope
- [ ] Receive and display telescope data
- [ ] Images download and display
- [ ] File logging works
- [ ] App handles background/foreground transitions
- [ ] App doesn't drain battery excessively
- [ ] Works on both iPhone and iPad
- [ ] Works in portrait and landscape
- [ ] Supports iOS dark mode (optional)

## Performance Optimization for iOS

### 1. Reduce Network Traffic
```cpp
// Poll less frequently on mobile
#ifdef Q_OS_IOS
    const int updateInterval = 2000; // 2 seconds
#else
    const int updateInterval = 1000; // 1 second
#endif
```

### 2. Battery Efficiency
```cpp
// Reduce timer frequency when in background
#ifdef Q_OS_IOS
void TelescopeGUI::handleApplicationStateChange(Qt::ApplicationState state) {
    if (state == Qt::ApplicationActive) {
        m_statusTimer->setInterval(1000);
    } else {
        m_statusTimer->setInterval(5000); // Slower updates in background
    }
}
#endif
```

### 3. Memory Management
```cpp
// Release large resources when not needed
void TelescopeGUI::clearImageCache() {
#ifdef Q_OS_IOS
    // iOS has stricter memory limits
    m_lastImage = QImage(); // Release image memory
    QPixmapCache::clear();
#endif
}
```

## Distribution Options

### TestFlight (Beta Testing)
1. Build release version in Xcode
2. Archive → Distribute → TestFlight
3. Upload to App Store Connect
4. Add testers via email

### App Store
1. Create App Store listing
2. Submit for review
3. Follow Apple's review guidelines

### Enterprise Distribution
- Requires Enterprise Developer Program ($299/year)
- Can distribute outside App Store
- Good for internal/scientific use

## Additional Resources

- [Qt for iOS Documentation](https://doc.qt.io/qt-6/ios.html)
- [Apple Human Interface Guidelines](https://developer.apple.com/design/human-interface-guidelines/ios)
- [Qt iOS Platform Notes](https://doc.qt.io/qt-6/ios-platform-plugin.html)

## Next Steps

1. Create iOS-specific `.pro` file
2. Add Info.plist with permissions
3. Test in iOS Simulator
4. Adapt UI for touch/mobile
5. Test on physical iOS device
6. Optimize for performance
7. Prepare for distribution
