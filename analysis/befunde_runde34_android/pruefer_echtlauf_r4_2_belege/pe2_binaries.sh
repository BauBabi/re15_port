#!/usr/bin/env bash
# Pruefer echtlauf R4-2: PC-Binaries v0.8.19 aus dem ARCHIV (nur lesen) holen.
# Split-Saetze per sha256 gegen das Archiv-SUMS, zusammenfuehren (zip -s 0, schreibt nur in den eigenen Ordner),
# NUR die Binaries entpacken (unzip setzt die mtime aus dem Eintrag; kein touch).
set -euo pipefail
A=/c/workspace/Re15Data/re15_packages_archiv/v0.8.19
W=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android/build/r34a/pruefer_e2/bin
export PATH="/c/msys64/usr/bin:$PATH"          # zip liegt in msys64/usr/bin
rm -rf "$W"; mkdir -p "$W/linux" "$W/win" "$W/tmp"
( cd "$A" && grep -E 'linux_steamdeck_x64|win64' SHA256SUMS.txt | sha256sum -c - )
zip -q -s 0 "$A/re15_port_v0.8.19_linux_steamdeck_x64.zip" --out "$W/tmp/linux.zip"
zip -q -s 0 "$A/re15_port_v0.8.19_win64.zip" --out "$W/tmp/win.zip"
unzip -Z1 "$W/tmp/linux.zip" | grep -E '/re15_pc$'
unzip -Z1 "$W/tmp/win.zip"   | grep -E '/re15_pc\.exe$'
unzip -q -j -o "$W/tmp/linux.zip" 're15_port_v0.8.19/re15_pc' -d "$W/linux"
unzip -q -j -o "$W/tmp/win.zip"   're15_port_v0.8.19/re15_pc.exe' -d "$W/win"
rm -rf "$W/tmp"
ls -la --time-style=full-iso "$W/linux/re15_pc" "$W/win/re15_pc.exe"
sha256sum "$W/win/re15_pc.exe" "$W/linux/re15_pc"
