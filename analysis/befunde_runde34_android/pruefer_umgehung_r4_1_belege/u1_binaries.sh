#!/usr/bin/env bash
# Pruefer UMGEHUNG R4-1: Linux-Binary v0.8.19 aus dem ARCHIV (nur gelesen) nach build/r34a/pruefer_u1/bin/linux/ -
# wie android_gate_r4_belege/r4_binaries.sh (Split-Satz kopieren, sha256 gegen die Archiv-SUMS, zip -s 0, nur das
# Binary entpacken, unzip stellt die Original-mtime her; KEIN touch). Rueckgabe != 0 bei jedem Fehler.
set -euo pipefail
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
ARCH=C:/workspace/Re15Data/re15_packages_archiv/v0.8.19
W=$BAUM/build/r34a/pruefer_u1/bin
ZIP=/c/msys64/usr/bin/zip; UNZIP=/c/msys64/usr/bin/unzip
rm -rf "$W"; mkdir -p "$W/satz"
for f in re15_port_v0.8.19_linux_steamdeck_x64.z01 re15_port_v0.8.19_linux_steamdeck_x64.zip; do cp -p "$ARCH/$f" "$W/satz/$f"; done
( cd "$W/satz" && grep -E 'linux_steamdeck' "$ARCH/SHA256SUMS.txt" | sha256sum -c - )
( cd "$W/satz" && $ZIP -q -s 0 re15_port_v0.8.19_linux_steamdeck_x64.zip --out linux_ganz.zip )
( cd "$W/satz" && $UNZIP -q -o linux_ganz.zip 're15_port_v0.8.19/re15_pc' -d ex_linux )
mkdir -p "$W/linux"
cp -p "$W/satz/ex_linux/re15_port_v0.8.19/re15_pc" "$W/linux/re15_pc"
rm -rf "$W/satz"
ls -la --time-style=full-iso "$W/linux/re15_pc"; sha256sum "$W/linux/re15_pc"
