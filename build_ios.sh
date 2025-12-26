#!/bin/bash
# build_ios.sh - Build script for iOS deployment

set -e  # Exit on error

# Configuration
APP_NAME="TelescopeMonitor"
PROJECT_FILE="TelescopeMonitor_iOS.pro"
QT_VERSION="6.9.3"  # Change to your Qt version
QT_PATH="$HOME/Qt/$QT_VERSION/ios"
BUILD_TYPE="release"  # or "debug"
TARGET="device"  # or "simulator"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}iOS Build Script for $APP_NAME${NC}"
echo -e "${GREEN}========================================${NC}"

# Check if Qt is installed
if [ ! -d "$QT_PATH" ]; then
    echo -e "${RED}Error: Qt iOS not found at $QT_PATH${NC}"
    echo "Please install Qt for iOS or update QT_PATH variable"
    exit 1
fi

echo -e "${YELLOW}Qt Path: $QT_PATH${NC}"

# Set up environment
export PATH="$QT_PATH/bin:$PATH"
export QMAKESPEC=macx-ios-clang

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="debug"
            shift
            ;;
        --release)
            BUILD_TYPE="release"
            shift
            ;;
        --simulator)
            TARGET="simulator"
            shift
            ;;
        --device)
            TARGET="device"
            shift
            ;;
        --clean)
            echo -e "${YELLOW}Cleaning build directory...${NC}"
            rm -rf build_ios
            echo -e "${GREEN}Clean complete${NC}"
            exit 0
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  --debug       Build debug version"
            echo "  --release     Build release version (default)"
            echo "  --simulator   Build for iOS Simulator"
            echo "  --device      Build for iOS Device (default)"
            echo "  --clean       Remove build directory"
            echo "  --help        Show this help"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            exit 1
            ;;
    esac
done

# Create build directory
BUILD_DIR="build_ios_${TARGET}_${BUILD_TYPE}"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo -e "${YELLOW}Build Directory: $BUILD_DIR${NC}"
echo -e "${YELLOW}Build Type: $BUILD_TYPE${NC}"
echo -e "${YELLOW}Target: $TARGET${NC}"

# Determine CONFIG flags
if [ "$TARGET" = "simulator" ]; then
    CONFIG_FLAGS="CONFIG+=iphonesimulator CONFIG+=simulator"
else
    CONFIG_FLAGS="CONFIG+=iphoneos CONFIG+=device"
fi

if [ "$BUILD_TYPE" = "debug" ]; then
    CONFIG_FLAGS="$CONFIG_FLAGS CONFIG+=debug"
else
    CONFIG_FLAGS="$CONFIG_FLAGS CONFIG+=release"
fi

echo -e "${GREEN}Step 1: Running qmake...${NC}"
qmake ../$PROJECT_FILE \
    -spec macx-ios-clang \
    $CONFIG_FLAGS

if [ $? -ne 0 ]; then
    echo -e "${RED}qmake failed!${NC}"
    exit 1
fi

echo -e "${GREEN}Step 2: Building with make...${NC}"
make -j$(sysctl -n hw.ncpu)

if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

# Find the generated Xcode project
XCODE_PROJECT=$(find . -name "*.xcodeproj" -type d | head -n 1)

if [ -z "$XCODE_PROJECT" ]; then
    echo -e "${YELLOW}No Xcode project generated. This is normal for some Qt versions.${NC}"
    echo -e "${GREEN}Build complete! Check the build directory for output.${NC}"
else
    echo -e "${GREEN}Xcode project: $XCODE_PROJECT${NC}"
    echo -e "${YELLOW}You can now open this project in Xcode for further development.${NC}"
    
    # Option to open in Xcode
    read -p "Open in Xcode? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        open "$XCODE_PROJECT"
    fi
fi

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Build Complete!${NC}"
echo -e "${GREEN}========================================${NC}"

if [ "$TARGET" = "device" ]; then
    echo -e "${YELLOW}Next steps for device deployment:${NC}"
    echo "1. Connect your iOS device via USB"
    echo "2. Open the Xcode project: $XCODE_PROJECT"
    echo "3. Select your device in the device selector"
    echo "4. Click the 'Run' button to build and deploy"
    echo ""
    echo "Or use the deploy_ios.sh script for command-line deployment"
else
    echo -e "${YELLOW}Next steps for simulator:${NC}"
    echo "1. Open the Xcode project: $XCODE_PROJECT"
    echo "2. Select a simulator from the device selector"
    echo "3. Click the 'Run' button"
fi

cd ..
