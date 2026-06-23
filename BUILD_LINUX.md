# Building CelestronOriginMonitor on Linux

Linux builds use the existing qmake project file, matching the macOS-oriented
build path and leaving the Windows CMake project alone.

## Prerequisites on Debian/Ubuntu

```bash
sudo apt-get update
sudo apt-get install -y --no-install-recommends \
    build-essential \
    dpkg-dev \
    fakeroot \
    libstellarsolver-dev \
    libqt5websockets5-dev \
    pkg-config \
    qt5-qmake \
    qtbase5-dev \
    qtbase5-dev-tools
```

Ubuntu's packaged StellarSolver development library is currently built against
Qt 5, so the Linux package workflow uses Qt 5 qmake packages.

## Build locally

```bash
qmake CelestronOriginMonitor.pro CONFIG+=release CONFIG-=debug
make -j"$(nproc)"
```

The executable is created at:

```bash
build/CelestronOriginMonitor
```

## Build a Debian package locally

```bash
bash packaging/build-deb-qmake.sh
```

The resulting `.deb` package is written to the repository root. The package
script builds with qmake, stages the executable under `/usr/bin`, computes
runtime shared-library dependencies with `dpkg-shlibdeps`, then adjusts the
StellarSolver dependency so either `libstellarsolver2` or `libstellarsolver` can
satisfy it.

## Astrometry index files

For plate solving, install astrometry index files separately and place them in
one of the standard Linux locations searched by the application:

- `/usr/share/astrometry`
- `/usr/local/share/astrometry`

For the Origin telescope's approximately 75 arcminute field of view, use
`index-4108.fits` through `index-4110.fits`.
