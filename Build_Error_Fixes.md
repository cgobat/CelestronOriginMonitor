# Fixing Your Build Errors

## The Problems in Your Build

Your build output shows **three main issues**:

### ❌ **Error 1: Info.plist Not Found**
```
WARNING: Could not resolve Info.plist: 'ios/Info.plist'
error: Cannot code sign because the target does not have an Info.plist file
```

### ❌ **Error 2: Duplicate LaunchScreen.storyboard**
```
error: Multiple commands produce conflicting outputs
note: /Users/jonathan/.../LaunchScreen.storyboardc/ (for task: ["LinkStoryboards"])
warning: duplicate output file '.../LaunchScreen.storyboardc'
```

### ⚠️ **Warning 3: Traditional headermap style**
```
warning: Traditional headermap style is no longer supported
```

---

## Quick Fix (5 minutes)

### **Step 1: Check Your Directory Structure**

Your project should look like this:
```
OriginMonitor-iOS/
├── TelescopeMonitor_iOS.pro        ← Your project file
├── Info.plist                       ← Must be at ROOT level
├── main.cpp
├── TelescopeGUI.cpp
├── OriginBackend.cpp
├── (other .cpp/.hpp files)
└── ios/                            ← Resources folder
    ├── LaunchScreen.storyboard
    └── Assets.xcassets/
```

**The Problem:** Your `Info.plist` is probably in the wrong location or doesn't exist.

### **Step 2: Place Info.plist in the RIGHT Location**

```bash
cd /Users/jonathan/OriginMonitor-iOS

# If you have Info.plist in ios/ folder, move it:
mv ios/Info.plist ./Info.plist

# Or copy the one I provided:
# (Use the Info.plist file from the outputs)
```

The `.pro` file references it as:
```qmake
QMAKE_INFO_PLIST = $$PWD/Info.plist
```

This means it expects `Info.plist` at the **project root**, not in `ios/` subdirectory.

### **Step 3: Fix the LaunchScreen Duplicate Issue**

**Option A: Minimal LaunchScreen (Recommended)**

If you don't need a fancy launch screen, create a simple one:

```bash
cd /Users/jonathan/OriginMonitor-iOS/ios
```

Create a minimal `LaunchScreen.storyboard`:
```xml
<?xml version="1.0" encoding="UTF-8"?>
<document type="com.apple.InterfaceBuilder3.CocoaTouch.Storyboard.XIB" version="3.0" toolsVersion="32" targetRuntime="iOS.CocoaTouch" propertyAccessControl="none" useAutolayout="YES" launchScreen="YES" useTraitCollections="YES" useSafeAreas="YES" colorMatched="YES">
    <device id="retina6_1" orientation="portrait" appearance="light"/>
    <dependencies>
        <deployment identifier="iOS"/>
        <plugIn identifier="com.apple.InterfaceBuilder.IBCocoaTouchPlugin"/>
    </dependencies>
    <scenes>
        <scene sceneID="EHf-IW-A2E">
            <objects>
                <viewController id="01J-lp-oVM" sceneMemberID="viewController">
                    <view key="view" contentMode="scaleToFill" id="Ze5-6b-2t3">
                        <rect key="frame" x="0.0" y="0.0" width="414" height="896"/>
                        <autoresizingMask key="autoresizingMask" widthSizable="YES" heightSizable="YES"/>
                        <color key="backgroundColor" white="1" alpha="1" colorSpace="custom" customColorSpace="genericGamma22GrayColorSpace"/>
                    </view>
                </viewController>
                <placeholder placeholderIdentifier="IBFirstResponder" id="iYj-Kq-Ea1" userLabel="First Responder" sceneMemberID="firstResponder"/>
            </objects>
        </scene>
    </scenes>
</document>
```

**Option B: Remove LaunchScreen Entirely (Fastest)**

In your `.pro` file, comment out or remove:
```qmake
# QMAKE_IOS_LAUNCH_SCREEN = $$PWD/ios/LaunchScreen.storyboard
```

And in `Info.plist`, remove:
```xml
<!-- Remove or comment out:
<key>UILaunchStoryboardName</key>
<string>LaunchScreen</string>
-->
```

### **Step 4: Use the Fixed .pro File**

Replace your current `.pro` file with `TelescopeMonitor_iOS_Fixed.pro`:

```bash
cd /Users/jonathan/OriginMonitor-iOS
cp TelescopeMonitor_iOS.pro TelescopeMonitor_iOS.pro.backup
cp TelescopeMonitor_iOS_Fixed.pro TelescopeMonitor_iOS.pro
```

**Key fixes in the new .pro file:**
1. ✅ Uses `$$PWD/Info.plist` (absolute path)
2. ✅ Uses `QMAKE_IOS_LAUNCH_SCREEN` instead of `QMAKE_BUNDLE_DATA` for launch screen
3. ✅ Fixes duplicate resource bundling
4. ✅ Adds proper Xcode settings

### **Step 5: Clean and Rebuild**

```bash
cd /Users/jonathan/OriginMonitor-iOS

# Clean everything
rm -rf build_ios_*
rm -rf *.xcodeproj

# Rebuild
sh build_ios.sh
```

---

## Detailed Explanation of Fixes

### **Fix 1: Info.plist Path**

**The Problem:**
```qmake
QMAKE_INFO_PLIST = ios/Info.plist  ❌ Relative path doesn't work
```

**The Solution:**
```qmake
QMAKE_INFO_PLIST = $$PWD/Info.plist  ✅ Absolute path
```

`$$PWD` expands to your project's full path, making the reference unambiguous.

### **Fix 2: Launch Screen Duplication**

**The Problem:** You're bundling the launch screen twice:
```qmake
# This bundles it once:
app_launch_images.files = $$PWD/ios/LaunchScreen.storyboard
QMAKE_BUNDLE_DATA += app_launch_images

# And this bundles it again:
UILaunchStoryboardName = LaunchScreen (in Info.plist)
```

**The Solution:** Use only ONE method:
```qmake
# Method 1: Let qmake handle it (Recommended)
QMAKE_IOS_LAUNCH_SCREEN = $$PWD/ios/LaunchScreen.storyboard

# OR Method 2: Manual bundling
app_launch_images.files = $$PWD/ios/LaunchScreen.storyboard
QMAKE_BUNDLE_DATA += app_launch_images

# NOT BOTH!
```

### **Fix 3: Header Search Paths**

The warning about "traditional headermap style" is addressed by:
```qmake
QMAKE_CXXFLAGS += -DALWAYS_SEARCH_USER_PATHS=NO
```

---

## Verification Checklist

After making these changes, verify:

```bash
# 1. Info.plist exists at root
ls -la Info.plist
# Should show: Info.plist

# 2. Info.plist is valid XML
plutil -lint Info.plist
# Should show: Info.plist: OK

# 3. Launch screen exists (if using it)
ls -la ios/LaunchScreen.storyboard

# 4. Clean build directory exists
ls -la build_ios_device_release
# Should be empty or not exist before building

# 5. Build
sh build_ios.sh
```

---

## Alternative: Start Fresh

If you're still having issues, start completely fresh:

```bash
# 1. Backup your source files
mkdir ~/telescope_backup
cp *.cpp *.hpp ~/telescope_backup/

# 2. Delete EVERYTHING in build directory
cd /Users/jonathan/OriginMonitor-iOS
rm -rf build_ios_*
rm -rf *.xcodeproj
rm -rf *.xcworkspace

# 3. Use the FIXED project file
cp TelescopeMonitor_iOS_Fixed.pro TelescopeMonitor_iOS.pro

# 4. Copy the FIXED Info.plist to root directory
cp Info.plist ./   # (the one from outputs)

# 5. Create minimal or remove LaunchScreen

# 6. Build
sh build_ios.sh --clean
sh build_ios.sh
```

---

## Testing the Fix

After building successfully, you should see:

```
✅ qmake runs without Info.plist warnings
✅ xcodebuild completes without errors
✅ .app file created in build_ios_device_release/Release-iphoneos/
```

Success output will look like:
```
Build Succeeded
** BUILD SUCCEEDED **
```

---

## Common Mistakes to Avoid

| ❌ Wrong | ✅ Right |
|----------|----------|
| `QMAKE_INFO_PLIST = ios/Info.plist` | `QMAKE_INFO_PLIST = $$PWD/Info.plist` |
| Info.plist in `ios/` folder | Info.plist at project root |
| Using both launch screen methods | Pick ONE method only |
| Multiple QMAKE_BUNDLE_DATA for same file | Bundle each file once |

---

## Still Not Working?

### Check File Permissions
```bash
chmod 644 Info.plist
chmod 644 ios/LaunchScreen.storyboard
```

### Verify Qt Version
```bash
~/Qt/6.9.3/ios/bin/qmake --version
# Should match your Qt installation
```

### Check Xcode Version
```bash
xcodebuild -version
# Should be Xcode 15 or 16
```

### Look for Hidden Characters
```bash
file Info.plist
# Should show: XML 1.0 document text, UTF-8 Unicode text
```

---

## Summary

**Three critical fixes:**
1. ✅ Move `Info.plist` to project root, use `$$PWD` in .pro file
2. ✅ Use only ONE launch screen bundling method
3. ✅ Use the fixed `.pro` file with proper settings

Apply these fixes, clean build, and you should be good to go!
