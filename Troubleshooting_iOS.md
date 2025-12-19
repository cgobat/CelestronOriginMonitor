# iOS Port Troubleshooting Guide

## Build Issues

### Problem: "Qt not found" or "qmake: command not found"

**Symptoms:**
```
bash: qmake: command not found
```

**Solutions:**
1. Check Qt installation:
```bash
ls ~/Qt/*/ios/bin/qmake
```

2. Set Qt path explicitly:
```bash
export QTDIR=~/Qt/6.6.0/ios
export PATH=$QTDIR/bin:$PATH
```

3. Update `build_ios.sh` with your Qt version:
```bash
QT_VERSION="6.6.0"  # Change this line
```

---

### Problem: "No provisioning profile found"

**Symptoms:**
```
error: No profiles for 'com.yourcompany.TelescopeMonitor' were found
```

**Solutions:**
1. In Xcode:
   - Select project in navigator
   - Go to "Signing & Capabilities"
   - Check "Automatically manage signing"
   - Select your Apple ID team

2. Or create a free Apple Developer account:
   - Xcode → Preferences → Accounts
   - Click "+" → Add Apple ID
   - Sign in with your Apple ID

---

### Problem: Build fails with linking errors

**Symptoms:**
```
Undefined symbols for architecture arm64:
  "_SpiceFunction", referenced from:
```

**Solutions:**
1. Check that all source files are in .pro file
2. Ensure libraries are built for iOS (arm64)
3. For SPICE library, you may need to build it specifically for iOS:

```bash
# Build SPICE for iOS (example)
cd cspice/src/cspice
# Configure for iOS cross-compilation
# This varies by library
```

---

### Problem: Info.plist errors

**Symptoms:**
```
error: The Info.plist file does not exist
```

**Solutions:**
1. Ensure `ios/Info.plist` exists in your project directory
2. Check .pro file has correct path:
```qmake
QMAKE_INFO_PLIST = ios/Info.plist
```

3. Verify file permissions:
```bash
ls -la ios/Info.plist
chmod 644 ios/Info.plist
```

---

## Runtime Issues

### Problem: App crashes immediately on launch

**Check 1: Console Logs**
In Xcode, open Console (View → Debug Area → Activate Console)

Common crash reasons:
- Missing bundled resources
- Invalid Info.plist configuration
- Missing library dependencies

**Check 2: Missing Resources**
```bash
# Verify resources are bundled
cd build_ios_*/
find . -name "*.bsp"  # Should find your SPICE kernels
```

**Check 3: File Paths**
Ensure code uses proper iOS paths:
```cpp
// WRONG - hardcoded macOS path
QString path = "/Users/jonathan/spice_kernels/";

// RIGHT - platform-aware path
QString path = PlatformUtils::getKernelPath();
```

---

### Problem: "Network connection failed"

**Symptoms:**
- Cannot discover telescopes
- WebSocket won't connect
- HTTP requests timeout

**Solutions:**

1. Check Info.plist permissions:
```xml
<key>NSLocalNetworkUsageDescription</key>
<string>Required for telescope discovery</string>

<key>NSAppTransportSecurity</key>
<dict>
    <key>NSAllowsArbitraryLoads</key>
    <true/>
</dict>
```

2. Grant permissions on device:
   - Settings → Privacy → Local Network → Your App → Enable

3. Ensure device and telescope on same WiFi network

4. Test on real device (simulator has limited networking)

5. Check firewall settings on telescope

---

### Problem: "Permission denied" for local network (iOS 14+)

**Symptoms:**
- Network discovery doesn't work
- No permission dialog appears

**Solutions:**

1. Delete and reinstall app (forces permission prompt)

2. Manually trigger permission:
```cpp
// In your discovery code
#ifdef Q_OS_IOS
    // Trigger local network permission on iOS 14+
    udpSocket->bind(QHostAddress::AnyIPv4, 55555);
#endif
```

3. Add to Info.plist:
```xml
<key>NSBonjourServices</key>
<array>
    <string>_http._tcp</string>
</array>
```

---

### Problem: Touch targets too small

**Symptoms:**
- Buttons hard to tap
- Need to tap multiple times

**Solutions:**

1. Apply Apple's 44pt minimum:
```cpp
#ifdef Q_OS_IOS
button->setMinimumHeight(44);
#endif
```

2. Increase font sizes:
```cpp
QFont font = button->font();
font.setPointSize(16);  // Larger for iOS
button->setFont(font);
```

3. Add padding:
```cpp
button->setStyleSheet("padding: 12px;");
```

---

### Problem: UI elements cut off or overlapping

**Symptoms:**
- Text doesn't fit in labels
- Buttons overlap
- Content extends off screen

**Solutions:**

1. Use scroll areas for content:
```cpp
#ifdef Q_OS_IOS
QScrollArea *scroll = new QScrollArea();
scroll->setWidget(yourWidget);
#endif
```

2. Test on multiple screen sizes:
   - iPhone SE (small)
   - iPhone 15 Pro (medium)
   - iPad Pro (large)

3. Use layouts properly:
```cpp
// Use spacers
layout->addStretch();

// Set size policies
widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
```

---

### Problem: App uses too much battery

**Symptoms:**
- Device gets warm
- Battery drains quickly
- iOS kills app in background

**Solutions:**

1. Reduce timer frequency:
```cpp
#ifdef Q_OS_IOS
    statusTimer->setInterval(2000);  // 2 sec instead of 1
#endif
```

2. Handle background state:
```cpp
void handleAppStateChange(Qt::ApplicationState state) {
    if (state != Qt::ApplicationActive) {
        // Pause non-critical operations
        statusTimer->stop();
    }
}
```

3. Release resources when not needed:
```cpp
void cleanup() {
    m_lastImage = QImage();  // Free memory
    QPixmapCache::clear();
}
```

---

### Problem: Images won't display

**Symptoms:**
- Image area blank
- "Failed to load image" errors

**Solutions:**

1. Check image format support:
```cpp
qDebug() << "Supported formats:" << QImageReader::supportedImageFormats();
```

2. Check memory limits:
```cpp
#ifdef Q_OS_IOS
if (imageData.size() > 10 * 1024 * 1024) {  // 10MB
    qWarning() << "Image too large for iOS";
}
#endif
```

3. Scale images for mobile:
```cpp
QImage scaledImage = originalImage.scaled(
    1024, 1024,  // Max dimensions
    Qt::KeepAspectRatio,
    Qt::SmoothTransformation
);
```

---

## Deployment Issues

### Problem: "Unable to install app" on device

**Solutions:**

1. Check device iOS version matches deployment target:
   - .pro file: `QMAKE_IOS_DEPLOYMENT_TARGET = 12.0`
   - Device must be iOS 12.0 or later

2. Trust developer certificate on device:
   - Settings → General → VPN & Device Management
   - Tap your certificate → Trust

3. Check device storage space

4. Clean build:
```bash
./build_ios.sh --clean
./build_ios.sh --device --release
```

---

### Problem: App works in simulator but not on device

**Solutions:**

1. Check architecture:
   - Device uses ARM64
   - Ensure libraries compiled for ARM64

2. Check file paths:
   - Simulator may have different file system permissions
   - Test with PlatformUtils on actual device

3. Network differences:
   - Simulator uses Mac's network
   - Device has its own network stack

---

### Problem: TestFlight upload fails

**Symptoms:**
```
ERROR ITMS-90000: "This bundle is invalid"
```

**Solutions:**

1. Check bundle ID matches App Store Connect
2. Ensure version and build numbers are incremented
3. Validate archive before upload:
   - Xcode → Window → Organizer
   - Select archive → Validate App

4. Check for missing icons:
   - Need all required icon sizes
   - Use Assets.xcassets

---

## Performance Issues

### Problem: App is slow/laggy on device

**Solutions:**

1. Profile with Instruments:
   - Xcode → Product → Profile
   - Select "Time Profiler"
   - Identify bottlenecks

2. Reduce UI updates:
```cpp
// Bad: Update every item individually
for (item : items) {
    list->addItem(item);
}

// Good: Batch update
list->addItems(items);
```

3. Use Qt Quick for better mobile performance (major refactor)

---

### Problem: High memory usage

**Solutions:**

1. Monitor memory in Xcode:
   - Debug → Memory Report

2. Release large objects:
```cpp
m_imageCache.clear();
QPixmapCache::clear();
```

3. Use weak references where appropriate

4. Implement image pagination/lazy loading

---

## Debugging Tips

### Enable Debug Logging

In your .pro file:
```qmake
CONFIG(debug, debug|release) {
    DEFINES += QT_MESSAGELOGCONTEXT
    DEFINES += DEBUG_NETWORK
    DEFINES += DEBUG_IMAGES
}
```

In code:
```cpp
#ifdef DEBUG_NETWORK
qDebug() << "Network:" << message;
#endif
```

### View Console Logs

**Option 1: Xcode Console**
- Run app from Xcode
- View → Debug Area → Activate Console

**Option 2: Mac Console App**
- Open Console.app
- Select your device
- Filter by process name

**Option 3: Command Line**
```bash
# For simulator
xcrun simctl spawn booted log stream --predicate 'process == "TelescopeMonitor"'

# For device (requires device connected)
idevicesyslog
```

### Crash Logs

**Location on Mac:**
```
~/Library/Logs/CrashReporter/MobileDevice/[DeviceName]/
```

**Access via Xcode:**
1. Window → Devices and Simulators
2. Select your device
3. View Device Logs

### Remote Debugging

For WebSocket/network issues:
```bash
# Use Charles Proxy or Wireshark
# Configure iOS device to use Mac as proxy
```

---

## Common Error Messages

### "ld: library not found for -lCSpice"
**Solution:** SPICE library not built for iOS architecture
- Build SPICE for arm64
- Or comment out SPICE features temporarily

### "dyld: Library not loaded"
**Solution:** Missing runtime library
- Check Qt installation includes all modules
- Verify LIBS in .pro file

### "App Transport Security blocked"
**Solution:** Add ATS exception in Info.plist

### "Operation not permitted"
**Solution:** Missing Info.plist permission description

---

## Getting More Help

1. **Enable verbose qmake output:**
```bash
qmake -v
qmake CONFIG+=verbose
```

2. **Qt Forums:**
- https://forum.qt.io/category/14/qt-for-mobile
- Tag posts with: qt, ios, qmake

3. **Apple Developer Forums:**
- https://developer.apple.com/forums/

4. **Stack Overflow:**
- Tags: `qt`, `ios`, `qt-ios`, `qmake`

5. **Qt Bug Tracker:**
- https://bugreports.qt.io
- Search before reporting

---

## Prevention Checklist

Before asking for help, verify:

- [ ] Latest Qt version installed
- [ ] Latest Xcode version
- [ ] All permissions in Info.plist
- [ ] Platform-aware file paths used
- [ ] Tested on both simulator and device
- [ ] Console logs checked
- [ ] Clean build attempted
- [ ] Device has free storage space
- [ ] iOS version compatible with deployment target

---

## Still Stuck?

If none of these solutions work:

1. **Create minimal test case:**
   - Strip down to simplest version that shows problem
   - Easier to diagnose

2. **Compare with Qt examples:**
   - Check Qt's iOS examples
   - See how they handle similar features

3. **Check Qt version notes:**
   - Known issues for your Qt version
   - May need to upgrade/downgrade

4. **Consider posting on forums with:**
   - Qt version
   - Xcode version
   - iOS version
   - Full error message
   - Minimal code example
   - What you've tried already
