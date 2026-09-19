#!/usr/bin/env bash
# Struktur-Layout x86_64 vs. aarch64 (laeuft in einem amd64-Debian-Container).
set -uo pipefail
export DEBIAN_FRONTEND=noninteractive
apt-get update -qq >/dev/null 2>&1
apt-get install -y -qq --no-install-recommends gcc gcc-aarch64-linux-gnu libc6-dev libc6-dev-arm64-cross >/dev/null 2>&1 || { echo APT-FAIL; exit 1; }
SRC=/src/re15_port
echo "--- x86_64 ---"
gcc      -std=c11 -I$SRC/include -c $SRC/tools/layout_check.c -o /tmp/lx.o && echo "x86_64 LAYOUT OK"
echo "--- aarch64 ---"
aarch64-linux-gnu-gcc -std=c11 -I$SRC/include -c $SRC/tools/layout_check.c -o /tmp/la.o && echo "aarch64 LAYOUT OK"
echo "--- aarch64 -mstrict-align (haerteste Variante: HW-Unaligned aus) ---"
aarch64-linux-gnu-gcc -std=c11 -O2 -mstrict-align -I$SRC/include -c $SRC/tools/layout_check.c -o /tmp/las.o && echo "aarch64 -mstrict-align LAYOUT OK"
echo "--- armv7 (32-Bit ARM) Groessen, falls Compiler vorhanden ---"
if command -v arm-linux-gnueabihf-gcc >/dev/null 2>&1; then
  arm-linux-gnueabihf-gcc -std=c11 -I$SRC/include -c $SRC/tools/layout_check.c -o /tmp/lr.o && echo "armhf LAYOUT OK"
fi
echo "--- Engine fuer aarch64 mit -mstrict-align uebersetzen (traps auf unaligned) ---"
mkdir -p /tmp/sa
rc=0; : > /tmp/sa_err.txt
for f in $SRC/engine/src/*.c; do
  aarch64-linux-gnu-gcc -std=c11 -O2 -mstrict-align -I$SRC/include -DRE15_PLATFORM_PC=1 \
     "-DRE15_ASSETS_PATH=$SRC/shared_assets/PSX" -c "$f" -o "/tmp/sa/$(basename $f .c).o" 2>>/tmp/sa_err.txt || rc=1
done
echo "strict-align Objekte: $(ls /tmp/sa | wc -l)  rc=$rc  Fehler: $(grep -c 'error:' /tmp/sa_err.txt)"
grep 'error:' /tmp/sa_err.txt | head -5
echo LAYOUT-CHECK-DONE
