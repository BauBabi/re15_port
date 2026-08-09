#!/bin/bash
# Linux/Steam-Deck-Build fuer RE1.5-Port v0.1 — laeuft IN ubuntu:22.04 (glibc 2.35 <= SteamOS 3.5+).
# SDL2 2.28.5 statisch via FetchContent (wie Windows); Video-/Audio-Backends brauchen dev-Header.
set -e
export DEBIAN_FRONTEND=noninteractive
apt-get update -qq
apt-get install -y -qq build-essential cmake ninja-build ca-certificates \
  libgl1-mesa-dev libx11-dev libxext-dev libxrandr-dev libxi-dev libxcursor-dev \
  libxfixes-dev libxss-dev libxkbcommon-dev libwayland-dev libdecor-0-dev \
  libasound2-dev libpulse-dev libdbus-1-dev libudev-dev >/dev/null
cmake -S /src/re15_port -B /tmp/b -G Ninja -DRE15_BUILD_PC=ON -DRE15_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Release \
  -DRE15_ASSETS_PATH=/src/re15_port/shared_assets/PSX
cmake --build /tmp/b -j"$(nproc)"
cd /tmp/b && ctest --timeout 120 --output-on-failure 2>&1 | tail -3
mkdir -p /src/release/linux_out
cp /tmp/b/platform/pc/re15_pc /src/release/linux_out/
ldd /tmp/b/platform/pc/re15_pc > /src/release/linux_out/ldd.txt
echo LINUX-BUILD-OK
