# FIXING: Cannot code sign because target does not have Info.plist

## The Problem

Xcode is saying it cannot find your Info.plist file, even if it exists. This is because **qmake needs to explicitly tell Xcode where it is**.

## The Solution (Choose ONE)

### **Option A: Use Xcode Build Settings (RECOMMENDED)**

This is the most reliable method.

#### Step 1: Verify Info.plist Location

```bash
cd /Users/jonathan/OriginMonitor-iOS

# Check if Info.plist exists
ls -la Info.plist

# If it doesn't exist, create it from my provided file
# Copy Info.plist from outputs to your project root
```

The file MUST be at `/Users/jonathan/OriginMonitor-iOS/Info.plist`

#### Step 2: Update Your .pro File

Replace your `.pro` file with this critical section:

```qmake
ios {
    # CRITICAL: Set Info.plist for both qmake AND Xcode
    QMAKE_INFO_PLIST = $$PWD/Info.plist
    
    # Force Xcode to recognize it
    info_plist.name = INFOPLIST_FILE
    info_plist.value = $$PWD/Info.plist
    QMAKE_MAC_XCODE_SETTINGS += info_plist
}
```

The key is `QMAKE_MAC_XCODE_SETTINGS` - this passes the setting directly to Xcode.

#### Step 3: Clean and Rebuild

```bash
# Clean EVERYTHING
rm -rf build_ios_*
rm -rf *.xcodeproj
rm -rf *.xcworkspace

# Regenerate the Xcode project
~/Qt/6.9.3/ios/bin/qmake TelescopeMonitor_iOS.pro

# Build
make

# Or use your build script
sh build_ios.sh
```

---

### **Option B: Manually Edit Xcode Project (If Option A Fails)**

If the .pro file method doesn't work, manually fix the Xcode project:

#### Step 1: Generate Xcode Project

```bash
cd /Users/jonathan/OriginMonitor-iOS
~/Qt/6.9.3/ios/bin/qmake TelescopeMonitor_iOS.pro
```

This creates `TelescopeMonitor.xcodeproj`

#### Step 2: Open in Xcode

```bash
open TelescopeMonitor.xcodeproj
```

#### Step 3: Set Info.plist Path

1. In Xcode, select **TelescopeMonitor** project in the navigator (blue icon at top)
2. Select the **TelescopeMonitor** target
3. Go to **Build Settings** tab
4. Search for "info.plist" or "INFOPLIST_FILE"
5. Find **Packaging → Info.plist File**
6. Set the value to: `$(SRCROOT)/Info.plist`

Or set it to the absolute path:
```
/Users/jonathan/OriginMonitor-iOS/Info.plist
```

#### Step 4: Build from Xcode

Click the **Play** button or press **Cmd+B** to build.

---

### **Option C: Generate Info.plist Automatically (LAST RESORT)**

If you can't get your Info.plist recognized, let Xcode generate one:

#### In Your .pro File:

```qmake
ios {
    # Let Xcode generate Info.plist automatically
    generate_plist.name = GENERATE_INFOPLIST_FILE
    generate_plist.value = YES
    QMAKE_MAC_XCODE_SETTINGS += generate_plist
    
    # But also set minimum required values
    bundle_id.name = PRODUCT_BUNDLE_IDENTIFIER
    bundle_id.value = com.astronomy.telescopemonitor
    QMAKE_MAC_XCODE_SETTINGS += bundle_id
    
    display_name.name = PRODUCT_NAME
    display_name.value = "Telescope Monitor"
    QMAKE_MAC_XCODE_SETTINGS += display_name
}
```

**Warning:** This generates a minimal Info.plist. You'll lose custom permissions like network access!

---

## Verification Steps

After applying the fix:

### 1. Check qmake Output

```bash
~/Qt/6.9.3/ios/bin/qmake TelescopeMonitor_iOS.pro

# Should NOT show:
# WARNING: Could not resolve Info.plist
```

### 2. Check Generated Xcode Project

```bash
# Open the .xcodeproj in a text editor
cat TelescopeMonitor.xcodeproj/project.pbxproj | grep -i "infoplist"

# Should show a line like:
# INFOPLIST_FILE = /Users/jonathan/OriginMonitor-iOS/Info.plist
```

### 3. Build and Check for Error

```bash
sh build_ios.sh

# Should NOT show:
# error: Cannot code sign because the target does not have an Info.plist
```

---

## Debugging: Why Isn't It Working?

### Issue 1: File Path is Wrong

**Check:**
```bash
# What qmake thinks the path is
~/Qt/6.9.3/ios/bin/qmake -query

# Print the actual path used
echo "$$PWD/Info.plist" 
```

**Fix:**
Use absolute path if `$$PWD` isn't working:
```qmake
QMAKE_INFO_PLIST = /Users/jonathan/OriginMonitor-iOS/Info.plist
```

### Issue 2: Info.plist Has Syntax Errors

**Check:**
```bash
plutil -lint Info.plist

# Should output: Info.plist: OK
```

**Fix:**
If it shows errors, use the Info.plist I provided (it's validated).

### Issue 3: Multiple .pro Files

**Check:**
```bash
ls -la *.pro

# Make sure you're editing the RIGHT one
```

**Fix:**
Only have ONE .pro file, or specify which one to use:
```bash
qmake TelescopeMonitor_iOS.pro  # Explicit filename
```

### Issue 4: Xcode Cache

**Check:**
Xcode might be using cached build settings.

**Fix:**
```bash
# Clean Xcode derived data
rm -rf ~/Library/Developer/Xcode/DerivedData/*

# Clean project
rm -rf build_ios_*
rm -rf *.xcodeproj

# Rebuild
qmake TelescopeMonitor_iOS.pro
make
```

---

## The Complete Working .pro File

I've created `TelescopeMonitor_iOS_InfoPlistFix.pro` with ALL the fixes:

```qmake
ios {
    # Method 1: qmake setting
    QMAKE_INFO_PLIST = $$PWD/Info.plist
    
    # Method 2: Xcode build setting (THE KEY FIX)
    info_plist.name = INFOPLIST_FILE
    info_plist.value = $$PWD/Info.plist
    QMAKE_MAC_XCODE_SETTINGS += info_plist
    
    # Also enable automatic code signing
    automatic_signing.name = "CODE_SIGN_STYLE"
    automatic_signing.value = "Automatic"
    QMAKE_MAC_XCODE_SETTINGS += automatic_signing
}
```

---

## Testing Your Fix

### Test 1: qmake Shows No Warnings

```bash
~/Qt/6.9.3/ios/bin/qmake TelescopeMonitor_iOS.pro 2>&1 | grep -i warning

# Should be empty or no Info.plist warnings
```

### Test 2: Xcode Project Has INFOPLIST_FILE

```bash
grep -r "INFOPLIST_FILE" TelescopeMonitor.xcodeproj/

# Should show: INFOPLIST_FILE = /path/to/Info.plist
```

### Test 3: Build Succeeds

```bash
xcodebuild -project TelescopeMonitor.xcodeproj \
           -scheme TelescopeMonitor \
           -configuration Debug \
           -destination 'generic/platform=iOS Simulator' \
           build

# Should end with: ** BUILD SUCCEEDED **
```

---

## Summary: The Fix

**The key issue:** qmake sets `QMAKE_INFO_PLIST` but Xcode doesn't always see it.

**The solution:** Explicitly set `INFOPLIST_FILE` in `QMAKE_MAC_XCODE_SETTINGS`:

```qmake
info_plist.name = INFOPLIST_FILE
info_plist.value = $$PWD/Info.plist
QMAKE_MAC_XCODE_SETTINGS += info_plist
```

This forces qmake to pass the setting directly to Xcode's build system.

---

## Still Not Working?

Try this nuclear option:

```bash
cd /Users/jonathan/OriginMonitor-iOS

# 1. Delete everything
rm -rf build_ios_*
rm -rf *.xcodeproj
rm -rf *.xcworkspace

# 2. Use ABSOLUTE path in .pro file
# Edit TelescopeMonitor_iOS.pro:
#   QMAKE_INFO_PLIST = /Users/jonathan/OriginMonitor-iOS/Info.plist

# 3. Build with verbose output
~/Qt/6.9.3/ios/bin/qmake TelescopeMonitor_iOS.pro -spec macx-ios-clang CONFIG+=device
make -d | tee build.log

# 4. Search the log for Info.plist mentions
grep -i "info.plist" build.log
```

Send me the output if it still fails!
