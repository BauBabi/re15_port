#!/usr/bin/env bash
# =============================================================================
# RE1.5 Port — aarch64-(ARM64-)Portabilitaets-Build. LAEUFT IM CONTAINER.
# =============================================================================
#   MSYS_NO_PATHCONV=1 docker run --rm -v "/c/workspace/git/reAi_v2":/src \
#       debian:12 bash /src/re15_port/tools/arm64_build.sh
#
# WEG: Cross-Compile aus einem x86_64-Container (gcc-aarch64-linux-gnu), Tests
# ueber qemu-user (CMAKE_CROSSCOMPILING_EMULATOR). Gewaehlt statt eines nativen
# arm64-Containers, weil 364 Test-Executables + SDL2 dort komplett emuliert
# uebersetzt werden muessten; hier laeuft nur das AUSFUEHREN der Tests unter qemu.
#
# Es werden ZWEI Baeume gebaut, mit identischen Warn-Flags, damit die
# Warn-Differenz aarch64-vs-x86_64 belastbar ist (und nicht nur "mehr Warnungen").
# =============================================================================
set -uo pipefail

REPO=/src
OUT="$REPO/build_arm64_cross"
mkdir -p "$OUT"
WARN="-Wall -Wextra -Wcast-align=strict"

step() { echo "=== [$(date +%H:%M:%S)] $* ==="; }

step "apt"
export DEBIAN_FRONTEND=noninteractive
dpkg --add-architecture arm64
apt-get update -qq
apt-get install -y -qq --no-install-recommends \
    build-essential ninja-build git ca-certificates wget pkg-config file \
    gcc-aarch64-linux-gnu g++-aarch64-linux-gnu qemu-user-static \
    cmake \
    libx11-dev:arm64 libxext-dev:arm64 libxrandr-dev:arm64 libxcursor-dev:arm64 \
    libxi-dev:arm64 libxss-dev:arm64 libxfixes-dev:arm64 libwayland-dev:arm64 \
    libxkbcommon-dev:arm64 wayland-protocols libasound2-dev:arm64 \
    libpulse-dev:arm64 libdbus-1-dev:arm64 libudev-dev:arm64 \
    libgl-dev:arm64 libegl-dev:arm64 libglx-dev:arm64 \
    > "$OUT/apt.log" 2>&1
APT_RC=$?
echo "apt rc=$APT_RC"
if [[ $APT_RC != 0 ]]; then tail -30 "$OUT/apt.log"; fi
cmake --version | head -1
aarch64-linux-gnu-gcc --version | head -1
gcc --version | head -1
qemu-aarch64-static --version | head -1

# --------------------------------------------------------------------------
step "configure aarch64"
export PKG_CONFIG_LIBDIR=/usr/lib/aarch64-linux-gnu/pkgconfig:/usr/share/pkgconfig
cmake -S "$REPO/re15_port" -B /build/a64 -G Ninja \
      -DCMAKE_TOOLCHAIN_FILE="$REPO/re15_port/cmake/aarch64_linux_toolchain.cmake" \
      -DRE15_BUILD_PC=ON -DRE15_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_FLAGS="$WARN" \
      -DRE15_ASSETS_PATH="$REPO/re15_port/shared_assets/PSX" \
      > "$OUT/a64_configure.log" 2>&1
echo "configure rc=$?"; tail -15 "$OUT/a64_configure.log"

step "build aarch64"
cmake --build /build/a64 -j"$(nproc)" > "$OUT/a64_build.log" 2>&1
A64_RC=$?
echo "build rc=$A64_RC"
tail -25 "$OUT/a64_build.log"

if [[ $A64_RC == 0 ]]; then
  step "artefakte aarch64"
  file /build/a64/platform/pc/re15_pc                     | tee -a "$OUT/a64_artifacts.txt"
  ls -l /build/a64/platform/pc/re15_pc                    | tee -a "$OUT/a64_artifacts.txt"
  aarch64-linux-gnu-objdump -p /build/a64/platform/pc/re15_pc 2>/dev/null \
      | grep -i NEEDED                                    | tee -a "$OUT/a64_artifacts.txt"
  aarch64-linux-gnu-objdump -T /build/a64/platform/pc/re15_pc 2>/dev/null \
      | grep -oE 'GLIBC_[0-9.]+' | sort -uV | tail -1     | tee -a "$OUT/a64_artifacts.txt"
  cp /build/a64/platform/pc/re15_pc "$OUT/re15_pc.aarch64" 2>/dev/null

  step "ctest aarch64 (qemu-user)"
  ( cd /build/a64 && ctest --timeout 600 --output-on-failure ) > "$OUT/a64_ctest.log" 2>&1
  echo "ctest rc=$?"
  grep -aE "tests passed|Total Test time" "$OUT/a64_ctest.log"
  grep -aA40 "The following tests FAILED" "$OUT/a64_ctest.log" | head -60
fi

# --------------------------------------------------------------------------
step "configure+build x86_64 (Referenz, gleiche Warn-Flags)"
unset PKG_CONFIG_LIBDIR
cmake -S "$REPO/re15_port" -B /build/x64 -G Ninja \
      -DRE15_BUILD_PC=ON -DRE15_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_FLAGS="$WARN" \
      -DRE15_ASSETS_PATH="$REPO/re15_port/shared_assets/PSX" \
      > "$OUT/x64_configure.log" 2>&1
echo "configure rc=$?"
cmake --build /build/x64 -j"$(nproc)" > "$OUT/x64_build.log" 2>&1
echo "build rc=$?"
tail -5 "$OUT/x64_build.log"

step "ctest x86_64 (Referenz)"
( cd /build/x64 && ctest --timeout 600 --output-on-failure ) > "$OUT/x64_ctest.log" 2>&1
echo "ctest rc=$?"
grep -aE "tests passed|Total Test time" "$OUT/x64_ctest.log"

# --------------------------------------------------------------------------
step "warn-diff"
# Nur eigener Code (kein _deps/ = SDL2), nur die Warn-Zeile selbst, normalisiert.
norm() { grep -a "warning:" "$1" | grep -av "/_deps/" \
          | sed -E 's/^.*(\/src\/re15_port\/[^ :]+):([0-9]+):[0-9]+: warning: /\1:\2: /' \
          | sort -u; }
norm "$OUT/a64_build.log" > "$OUT/warn_a64.txt"
norm "$OUT/x64_build.log" > "$OUT/warn_x64.txt"
echo "aarch64-Warnungen (eigener Code): $(wc -l < "$OUT/warn_a64.txt")"
echo "x86_64-Warnungen  (eigener Code): $(wc -l < "$OUT/warn_x64.txt")"
# Vergleich ueber den Warnungs-TEXT (Zeilennummern sind gleich, Dateien auch)
cut -d' ' -f2- "$OUT/warn_a64.txt" | sort | uniq -c | sort -rn > "$OUT/warn_a64_kinds.txt"
cut -d' ' -f2- "$OUT/warn_x64.txt" | sort | uniq -c | sort -rn > "$OUT/warn_x64_kinds.txt"
comm -23 "$OUT/warn_a64.txt" "$OUT/warn_x64.txt" > "$OUT/warn_only_a64.txt"
comm -13 "$OUT/warn_a64.txt" "$OUT/warn_x64.txt" > "$OUT/warn_only_x64.txt"
echo "--- NUR auf aarch64: $(wc -l < "$OUT/warn_only_a64.txt")"
head -80 "$OUT/warn_only_a64.txt"
echo "--- NUR auf x86_64: $(wc -l < "$OUT/warn_only_x64.txt")"
head -20 "$OUT/warn_only_x64.txt"
echo "--- Warn-Arten aarch64 (Top 30)"
head -30 "$OUT/warn_a64_kinds.txt"

step "ENDE"
