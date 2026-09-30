#!/usr/bin/env bash
# Gegenpruefung R4-2: Linux-Binary v0.8.19 aus dem ARCHIV (nur gelesen) in die Sandbox (release/linux_out/re15_pc) -
# wie pruefer_umgehung_r4_1_belege/u1_binaries.sh (Split-Satz kopieren, sha256 gegen die Archiv-SUMS, zip -s 0, nur das
# Binary entpacken, unzip stellt die Original-mtime her; KEIN touch).
set -euo pipefail
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
ARCH=C:/workspace/Re15Data/re15_packages_archiv/v0.8.19
W=$BAUM/build/r34a/pruefer_u2/bin
S=$BAUM/build/r34a/pruefer_u2/sb
ZIP=/c/msys64/usr/bin/zip; UNZIP=/c/msys64/usr/bin/unzip
rm -rf "$W"; mkdir -p "$W/satz"
for f in re15_port_v0.8.19_linux_steamdeck_x64.z01 re15_port_v0.8.19_linux_steamdeck_x64.zip; do cp -p "$ARCH/$f" "$W/satz/$f"; done
( cd "$W/satz" && grep -E 'linux_steamdeck' "$ARCH/SHA256SUMS.txt" | sha256sum -c - )
( cd "$W/satz" && $ZIP -q -s 0 re15_port_v0.8.19_linux_steamdeck_x64.zip --out linux_ganz.zip )
( cd "$W/satz" && $UNZIP -q -o linux_ganz.zip 're15_port_v0.8.19/re15_pc' -d ex_linux )
mkdir -p "$S/release/linux_out"
cp -p "$W/satz/ex_linux/re15_port_v0.8.19/re15_pc" "$S/release/linux_out/re15_pc"
rm -rf "$W/satz"
ls -la --time-style=full-iso "$S/release/linux_out/re15_pc"; sha256sum "$S/release/linux_out/re15_pc"
