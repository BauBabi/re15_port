#!/bin/bash
set -e
export DEBIAN_FRONTEND=noninteractive
apt-get update -qq && apt-get install -y -qq build-essential cmake ninja-build ca-certificates zip \
  libgl1-mesa-dev libx11-dev libxext-dev libxrandr-dev libxi-dev libxcursor-dev \
  libxfixes-dev libxss-dev libxkbcommon-dev libwayland-dev libdecor-0-dev \
  libasound2-dev libpulse-dev libdbus-1-dev libudev-dev >/dev/null
cmake --build /src/release/lbuild -j"$(nproc)" 2>&1 | tail -2
cd /src/release/lbuild && ctest --timeout 120 2>&1 | tail -3
install -m 755 /src/release/linux_out/re15_pc /src/release/pkg-linux/re15_port_v0.1/re15_pc
chmod +x /src/release/pkg-linux/re15_port_v0.1/run.sh
rm -f /src/release/re15_port_v0.1_linux_steamdeck_x64.z* /src/release/re15_port_v0.1_win64.z*
cd /src/release/pkg-linux && zip -q -s 90m -r ../re15_port_v0.1_linux_steamdeck_x64.zip re15_port_v0.1
cd /src/release/pkg-win  && zip -q -s 90m -r ../re15_port_v0.1_win64.zip re15_port_v0.1
cd /src/release
unzip -t -q re15_port_v0.1_win64.zip > /dev/null 2>&1 || zip -T re15_port_v0.1_win64.zip
zip -T re15_port_v0.1_linux_steamdeck_x64.zip
sha256sum re15_port_v0.1_win64.z* re15_port_v0.1_linux_steamdeck_x64.z* > SHA256SUMS.txt
ls -la re15_port_v0.1_*.z* && echo PACKAGE-OK
