#!/usr/bin/env bash
set -euo pipefail

if [ "$#" -eq 0 ]; then
    echo "usage: $0 package.deb [package.deb ...]" >&2
    exit 2
fi

for deb_path in "$@"; do
    if [ ! -f "$deb_path" ]; then
        echo "not a file: $deb_path" >&2
        exit 2
    fi

    work_dir="$(mktemp -d)"
    trap 'rm -rf "$work_dir"' EXIT

    package_dir="$work_dir/package"
    dpkg-deb -R "$deb_path" "$package_dir"

    control_file="$package_dir/DEBIAN/control"
    if ! grep -Eq '^Depends:.*\blibstellarsolver2\b' "$control_file"; then
        echo "warning: $deb_path does not depend on libstellarsolver2; leaving unchanged" >&2
        rm -rf "$work_dir"
        trap - EXIT
        continue
    fi

    if grep -Eq '^Depends:.*\blibstellarsolver2\b.*\|[[:space:]]*libstellarsolver\b' "$control_file"; then
        echo "$deb_path already allows libstellarsolver as an alternative; leaving unchanged"
        rm -rf "$work_dir"
        trap - EXIT
        continue
    fi

    python3 - "$control_file" <<'PY'
from pathlib import Path
import re
import sys

control_path = Path(sys.argv[1])
control = control_path.read_text(encoding="utf-8")

# dpkg-shlibdeps emits libstellarsolver2 from the package shlibs metadata.
# Some distributions provide the same ABI under the unversioned package name
# libstellarsolver instead, so make that dependency satisfiable by either name.
control = re.sub(
    r"\blibstellarsolver2(\s*\([^)]*\))?",
    lambda match: f"libstellarsolver2{match.group(1) or ''} | libstellarsolver",
    control,
    count=1,
)

control_path.write_text(control, encoding="utf-8")
PY

    dpkg-deb -b "$package_dir" "$deb_path" >/dev/null
    rm -rf "$work_dir"
    trap - EXIT
    echo "updated StellarSolver dependency in $deb_path"
done
