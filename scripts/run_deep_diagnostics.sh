#!/usr/bin/env bash
set -e

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

echo "================================================================="
echo "   S-Shot Wayland Comprehensive Deep Diagnostic Suite"
echo "================================================================="

# 1. Desktop Entry Validation
echo -e "\n[1/5] Validating Desktop File..."
desktop-file-validate resources/io.github.uri_travoski.s-shot-wayland.desktop
echo "      [PASS] Desktop file complies with freedesktop.org specifications."

# 2. Dynamic Library Inspection (Zero Direct X11 Linking)
echo -e "\n[2/5] Inspecting Dynamic Binary Linkage (Pure Wayland Architecture)..."
X11_LIBS=$(readelf -d build/s-shot-wayland | grep -iE 'NEEDED.*(X11|Xfixes|Xtst|Xext)' || true)
if [ -n "$X11_LIBS" ]; then
    echo "      [FAIL] Direct X11 libraries detected in binary:"
    echo "$X11_LIBS"
    exit 1
else
    echo "      [PASS] Verified zero direct X11/Xfixes/Xtst libraries linked in executable."
fi

# 3. CLI Flags Verification
echo -e "\n[3/5] Verifying Command Line Options..."
VERSION_OUT=$(./build/s-shot-wayland --version | tr -d '\r')
if [[ "$VERSION_OUT" =~ "1.32" ]]; then
    echo "      [PASS] CLI --version output: $VERSION_OUT"
else
    echo "      [FAIL] CLI --version output mismatch: $VERSION_OUT"
    exit 1
fi

# 4. Headless QtTest Test Suite (44 Automated Test Cases)
echo -e "\n[4/5] Running Comprehensive 44-Case QtTest Suite..."
./build/test_wayland_editor

# 5. Resource and Memory Profiler
echo -e "\n[5/5] Profiling Memory Footprint and Single-Instance Socket IPC..."
python3 scripts/profile_resources.py

echo -e "\n================================================================="
echo "   All Deep Diagnostics and Tests PASSED with 100% Success!"
echo "================================================================="
