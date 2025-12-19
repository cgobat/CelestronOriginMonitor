# Quick Start Guide - iOS Port

## Prerequisites Checklist

Before starting, ensure you have:

- [ ] macOS computer (required for iOS development)
- [ ] Xcode (latest version from Mac App Store)
- [ ] Qt for iOS installed (Qt 6.5+ recommended)
- [ ] Apple Developer account (free for testing on device)
- [ ] iOS device or simulator

## Step-by-Step Setup

### 1. Verify Qt Installation

```bash
# Check if Qt iOS is installed
ls ~/Qt/*/ios/bin/qmake

# Should output something like:
# ~/Qt/6.6.0/ios/bin/qmake
```

If not found, install Qt for iOS:
1. Download Qt Online Installer from https://www.qt.io/download
2. During installation, select "iOS" component
3. Wait for installation to complete

### 2. Prepare Your Project

Copy these files into your existing project directory:

```
YourProject/
├── TelescopeMonitor_iOS.pro    (iOS project file)
├── PlatformUtils.hpp            (Platform abstraction)
├── build_ios.sh                 (Build script)
├── ios/
│   ├── Info.plist              (iOS configuration)
│   └── LaunchScreen.storyboard (Optional)
└── spice_kernels/              (Your SPICE data files)
    ├── c2025a6.bsp
    ├── naif0012.tls
    └── de440s.bsp
```

### 3. Apply Code Modifications

Integrate the suggested changes from:
- `OriginBackend_iOS_Modifications.cpp` into your `OriginBackend.cpp`
- `TelescopeGUI_iOS_Modifications.cpp` into your `TelescopeGUI.cpp`

Key changes to make:

**In OriginBackend.cpp:**
```cpp
// Add at top
#include "PlatformUtils.hpp"

// Replace file paths
void OriginBackend::initializeLogging() {
    QString logDir = PlatformUtils::getLogPath();
    // ... rest of implementation
}
```

**In TelescopeGUI.cpp:**
```cpp
// Add at top
#include "PlatformUtils.hpp"

// In setupUI()
#ifdef Q_OS_IOS
    // iOS-specific UI adjustments
    setStyleSheet("QPushButton { min-height: 44px; }");
#endif
```

### 4. Update Project File

Edit `TelescopeMonitor_iOS.pro`:

1. Change bundle identifier:
```qmake
QMAKE_TARGET_BUNDLE_PREFIX = com.yourcompany
```

2. Verify all source files are listed:
```qmake
SOURCES += \
    main.cpp \
    TelescopeGUI.cpp \
    # ... add all your .cpp files
```

3. Ensure SPICE kernels are included:
```qmake
spice_kernels.files = $$files($$PWD/spice_kernels/*.bsp)
```

### 5. Configure Info.plist

Edit `ios/Info.plist` to add your app information:

```xml
<key>CFBundleDisplayName</key>
<string>Your App Name</string>

<key>NSLocalNetworkUsageDescription</key>
<string>Your description for network access</string>
```

### 6. Build for Simulator (Easiest First Test)

```bash
# Make build script executable (first time only)
chmod +x build_ios.sh

# Build for simulator
./build_ios.sh --simulator --debug
```

### 7. Open in Xcode

```bash
# Navigate to build directory
cd build_ios_simulator_debug

# Find and open Xcode project
open *.xcodeproj
```

### 8. Run in Simulator

In Xcode:
1. Select a simulator from the device dropdown (e.g., "iPhone 15 Pro")
2. Click the ▶ (Run) button
3. Wait for simulator to launch

### 9. Test Basic Functionality

Once app launches:
- [ ] UI appears correctly
- [ ] Text is readable (not too small)
- [ ] Buttons are touchable
- [ ] Check for any crash logs in Xcode console

### 10. Build for Device

```bash
# Build for actual iOS device
./build_ios.sh --device --release
```

Then in Xcode:
1. Connect your iPhone/iPad via USB
2. Select your device from device dropdown
3. If prompted, trust the computer on your device
4. Click Run button
5. On device: Settings → General → VPN & Device Management → Trust your certificate

## Common Issues & Solutions

### Issue: "No profile for team 'XXX' matching"
**Solution:** 
1. In Xcode, select project in navigator
2. Go to "Signing & Capabilities" tab
3. Select your team from dropdown
4. Enable "Automatically manage signing"

### Issue: App crashes on launch
**Solution:**
1. Check Xcode console for error messages
2. Verify Info.plist has required permissions
3. Ensure all resources are bundled correctly
4. Check for missing libraries

### Issue: Cannot discover telescopes
**Solution:**
1. Check Info.plist has `NSLocalNetworkUsageDescription`
2. Ensure device and telescope on same WiFi
3. Grant local network permission when prompted
4. Try on real device (simulator networking is limited)

### Issue: Qt not found
**Solution:**
```bash
# Set Qt path explicitly
export QTDIR=~/Qt/6.6.0/ios
export PATH=$QTDIR/bin:$PATH
./build_ios.sh
```

### Issue: Build errors about missing files
**Solution:**
- Verify all source files are listed in .pro file
- Check that `#include` paths are correct
- Ensure SPICE libraries are built for iOS if used

## Testing Checklist

Before considering port complete:

- [ ] App launches without crashes
- [ ] All tabs are accessible
- [ ] Buttons respond to touch
- [ ] Can discover telescopes on network
- [ ] Can connect to telescope
- [ ] Can send commands
- [ ] Can receive telescope data
- [ ] Images display correctly
- [ ] Works in both portrait and landscape
- [ ] Works on both iPhone and iPad (if Universal)
- [ ] No excessive battery drain during use
- [ ] App handles going to background/foreground

## Next Steps

### For Development Testing:
1. Use TestFlight for beta distribution
2. Invite testers via email
3. Collect feedback and crash reports

### For Distribution:
1. Create App Store listing
2. Prepare screenshots and description
3. Submit for Apple review
4. Address any review feedback

### For Enterprise Use:
1. Consider Enterprise Developer Program
2. Allows distribution outside App Store
3. Good for scientific/research applications

## Useful Commands

```bash
# Clean build
./build_ios.sh --clean

# Debug build for simulator
./build_ios.sh --simulator --debug

# Release build for device
./build_ios.sh --device --release

# View device logs
xcrun simctl spawn booted log stream --predicate 'process == "TelescopeMonitor"'

# List available simulators
xcrun simctl list devices
```

## Resources

- Qt iOS Documentation: https://doc.qt.io/qt-6/ios.html
- Apple HIG: https://developer.apple.com/design/human-interface-guidelines/ios
- Qt Creator Manual: https://doc.qt.io/qtcreator/
- Xcode Help: https://developer.apple.com/xcode/

## Getting Help

If you encounter issues:

1. Check Qt forums: https://forum.qt.io
2. Apple Developer Forums: https://developer.apple.com/forums/
3. Stack Overflow tags: `qt`, `ios`, `qt-ios`

## Maintenance

Remember to:
- Update Qt version periodically
- Test on new iOS versions
- Update Info.plist permissions as iOS evolves
- Keep Xcode updated
- Renew developer certificates annually
