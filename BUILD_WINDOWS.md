# Building CelestronOriginMonitor on Windows (MinGW64)

## Prerequisites: MSYS2 + MinGW64

### 1. Install MSYS2

Download and install from https://www.msys2.org/ or:
```powershell
winget install MSYS2.MSYS2
```

### 2. Install build dependencies

Open **MSYS2 MinGW64** shell (not the MSYS shell) and run:

```bash
# Update package database
pacman -Syu

# Compiler and build tools
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-make

# Qt6 with required modules
pacman -S mingw-w64-x86_64-qt6-base mingw-w64-x86_64-qt6-websockets

# StellarSolver dependencies
pacman -S mingw-w64-x86_64-cfitsio mingw-w64-x86_64-gsl mingw-w64-x86_64-mman-win32
```

### 3. Build wcslib from source

wcslib is not available as an MSYS2 package. Download from
https://www.atnf.csiro.au/people/mcalabre/WCS/wcslib.html and build:

```bash
tar xf wcslib-8.4.tar.xz
cd wcslib-8.4

# MinGW's string.h declares wcsset() which conflicts with wcslib's.
# Create a wrapper header that hides the MinGW declaration:
mkdir -p /tmp/wcslib-override
cat > /tmp/wcslib-override/string.h << 'EOF'
#ifndef _WCSLIB_STRING_WRAPPER
#define _WCSLIB_STRING_WRAPPER
#define wcsset wcsset_mingw_hidden
#include_next <string.h>
#undef wcsset
#endif
EOF

# Compile all source files (autotools build doesn't work on MinGW)
mkdir -p build-mingw && cd build-mingw
SRCS="cel dis fitshdr lin log prj spc sph spx tab wcs wcsbth wcserr
      wcsfix wcshdr wcspih wcsprintf wcstrig wcsulex wcsunits wcsutil wcsutrn"
for s in $SRCS; do
    gcc -I/tmp/wcslib-override -I../C -I.. -O2 -DHAVE_SINCOS \
        -c ../C/$s.c -o $s.o
done
ar rcs libwcs.a *.o

# Install
cp libwcs.a /mingw64/lib/
mkdir -p /mingw64/include/wcslib
cp ../C/*.h /mingw64/include/wcslib/
cp ../wcsconfig.h /mingw64/include/wcslib/
cd ../..
```

### 4. Build StellarSolver from source

```bash
git clone https://github.com/rlancaste/stellarsolver.git
cd stellarsolver

# Build as static library with dllexport (not dllimport) and wcsset fix
cmake -B build -G "MinGW Makefiles" \
    -DCMAKE_INSTALL_PREFIX=/mingw64 \
    -DCMAKE_BUILD_TYPE=Release \
    -DWCSLIB_INCLUDE_DIR=/mingw64/include/wcslib \
    -DWCSLIB_LIBRARIES=/mingw64/lib/libwcs.a \
    -DCMAKE_CXX_FLAGS="-I/tmp/wcslib-override -Dstellarsolver_EXPORTS" \
    -DCMAKE_C_FLAGS="-I/tmp/wcslib-override -Dstellarsolver_EXPORTS"

cmake --build build -j$(nproc)
cmake --install build
cd ..
```

### 5. Astrometry index files (for plate solving)

Download index files from http://data.astrometry.net/ and place in one of:
- `%APPDATA%/CelestronOriginMonitor/astrometry/`
- The application directory under `astrometry/`

For the Origin telescope's ~75 arcminute FOV, you need `index-4108.fits` through `index-4110.fits`.

## Building CelestronOriginMonitor

From the **MSYS2 MinGW64** shell:

```bash
cd /path/to/OriginMonitor

cmake -B build-mingw -G "MinGW Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DSTELLARSOLVER_DIR=/mingw64

cmake --build build-mingw -j$(nproc)

# The executable is at build-mingw/CelestronOriginMonitor.exe
```

## Running

The executable can be run directly from the MSYS2 MinGW64 shell, or from
a regular Windows terminal if `C:\msys64\mingw64\bin` is in your PATH.

For standalone distribution, `windeployqt` runs automatically as a post-build
step (via CMake) to copy required Qt DLLs alongside the executable.

## Troubleshooting

- **"Qt6WebSockets not found"**: Install `mingw-w64-x86_64-qt6-websockets`
- **"stellarsolver.h not found"**: Set `-DSTELLARSOLVER_DIR=<path>` or install to `/mingw64`
- **wcsset conflict**: wcslib's `wcsset()` conflicts with MinGW's `string.h`.
  The build uses a wrapper `string.h` with `#include_next` to hide the MinGW version.
- **StellarSolver dllimport errors**: StellarSolver must be built with
  `-Dstellarsolver_EXPORTS` so that `STELLARSOLVER_API` uses `dllexport`.
  The OriginMonitor build uses `stellarsolver_static.h` to suppress decoration.
- **Application crashes on startup**: Ensure all required DLLs are in PATH or
  alongside the executable. Run from MSYS2 MinGW64 shell or use `windeployqt`.

## Notes

- CometObserver/CometTracker and error_plot.cpp are **not** part of the main GUI.
  They require CSPICE and libnova (separate dependencies) and are not built by default.
- Log files are written to `Documents/CelestronOriginLogs/`.
- The application discovers Celestron Origin telescopes on the local network via
  UDP broadcast and connects via WebSocket.
