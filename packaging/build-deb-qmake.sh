#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_dir="$(cd "$script_dir/.." && pwd)"
app_name="CelestronOriginMonitor"
package_name="celestron-origin-monitor"
version="$(awk -F= '/^[[:space:]]*VERSION[[:space:]]*=/{gsub(/[[:space:]]/, "", $2); print $2; exit}' "$repo_dir/CelestronOriginMonitor.pro")"
if [ -z "$version" ]; then
    version="1.0.0"
fi
arch="$(dpkg --print-architecture)"
work_dir="$repo_dir/build/linux-deb"
pkg_dir="$work_dir/package"
deb_path="$repo_dir/${package_name}_${version}_${arch}.deb"

find_qmake() {
    if command -v qmake >/dev/null 2>&1; then
        command -v qmake
        return
    fi
    if command -v qmake-qt5 >/dev/null 2>&1; then
        command -v qmake-qt5
        return
    fi
    echo "qmake was not found. Install qt5-qmake or provide qmake in PATH." >&2
    exit 1
}

qmake_bin="$(find_qmake)"
make_jobs="${MAKEFLAGS:-}"

rm -rf "$work_dir"
mkdir -p "$work_dir/build"

cd "$work_dir/build"
"$qmake_bin" "$repo_dir/CelestronOriginMonitor.pro" \
    CONFIG+=release CONFIG-=debug \
    DESTDIR="$work_dir/out" \
    OBJECTS_DIR="$work_dir/obj" \
    MOC_DIR="$work_dir/moc" \
    RCC_DIR="$work_dir/rcc" \
    UI_DIR="$work_dir/ui"
make ${make_jobs:--j"$(nproc)"}

built_app=""
for candidate in \
    "$work_dir/out/$app_name" \
    "$work_dir/build/$app_name" \
    "$work_dir/build/build/$app_name"
do
    if [ -x "$candidate" ]; then
        built_app="$candidate"
        break
    fi
done

if [ -z "$built_app" ]; then
    echo "Built executable was not found." >&2
    find "$work_dir" -maxdepth 4 -type f -name "$app_name" -print >&2
    exit 1
fi

install -Dm755 "$built_app" "$pkg_dir/usr/bin/$app_name"

mkdir -p "$pkg_dir/DEBIAN" "$work_dir/debian"
cat > "$work_dir/debian/control" <<CONTROL
Source: $package_name
Section: science
Priority: optional
Maintainer: Celestron Origin Monitor maintainers
Standards-Version: 4.7.0

Package: $package_name
Architecture: any
Depends: \${shlibs:Depends}
Description: Celestron Origin telescope monitor
 A Qt-based monitor and control application for Celestron Origin telescopes.
CONTROL

depends="$(cd "$work_dir" && dpkg-shlibdeps -O "package/usr/bin/$app_name" | sed 's/^shlibs:Depends=//')"

cat > "$pkg_dir/DEBIAN/control" <<CONTROL
Package: $package_name
Version: $version
Section: science
Priority: optional
Architecture: $arch
Maintainer: Celestron Origin Monitor maintainers
Depends: $depends
Description: Celestron Origin telescope monitor
 A Qt-based monitor and control application for Celestron Origin telescopes.
CONTROL

find "$pkg_dir" -type d -exec chmod 755 {} +
find "$pkg_dir" -type f -exec chmod 644 {} +
chmod 755 "$pkg_dir/usr/bin/$app_name"

dpkg-deb -b "$pkg_dir" "$deb_path" >/dev/null
bash "$script_dir/relax-stellarsolver-dep.sh" "$deb_path"

echo "$deb_path"
