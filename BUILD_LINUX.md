# Building CelestronOriginMonitor on Linux

## Prerequisites on Debian/Ubuntu

```bash
sudo apt-get update
sudo apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    dpkg-dev \
    fakeroot \
    libstellarsolver-dev \
    libqt5websockets5-dev \
    ninja-build \
    pkg-config \
    qtbase5-dev
```

The CMake project can build with Qt 5 or Qt 6. On Ubuntu 24.04, the packaged
StellarSolver development library is built against Qt 5, so the default Linux
package workflow installs Qt 5 development packages.

## Build locally

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

The executable is created at:

```bash
build/CelestronOriginMonitor
```

## Build a Debian package locally

```bash
cpack --config build/CPackConfig.cmake -G DEB
```

The resulting `.deb` package is written to the repository root.

## Astrometry index files

For plate solving, install astrometry index files separately and place them in
one of the standard Linux locations searched by the application:

- `/usr/share/astrometry`
- `/usr/local/share/astrometry`

For the Origin telescope's approximately 75 arcminute field of view, use
`index-4108.fits` through `index-4110.fits`.
