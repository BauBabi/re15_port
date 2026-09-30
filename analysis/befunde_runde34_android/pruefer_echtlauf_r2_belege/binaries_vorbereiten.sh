#!/usr/bin/env bash
# Pruefer echtlauf r2: PC-Binaries der v0.8.19-Pakete aus dem ARCHIV (nur gelesen) holen:
# Split-Saetze nach build/ kopieren, sha256 gegen die Archiv-SUMS, zusammenfuehren (zip -s 0), NUR die
# Binaries entpacken (unzip stellt die Original-mtime her) und nach release/win_out + release/linux_out
# legen (cp -p, KEIN touch). Rueckgabe != 0 bei jedem Fehler.
set -euo pipefail
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
ARCH=C:/workspace/Re15Data/re15_packages_archiv/v0.8.19
W=$BAUM/build/r34a/pruefer_echtlauf_r2/pakete
export PATH="/c/msys64/usr/bin:$PATH"     # zip liegt dort
rm -rf "$W"; mkdir -p "$W"
for f in re15_port_v0.8.19_win64.z01 re15_port_v0.8.19_win64.zip \
         re15_port_v0.8.19_linux_steamdeck_x64.z01 re15_port_v0.8.19_linux_steamdeck_x64.zip; do
    cp -p "$ARCH/$f" "$W/$f"
done
( cd "$W" && grep -E 'win64|linux_steamdeck' "$ARCH/SHA256SUMS.txt" | sha256sum -c - )
( cd "$W" && zip -q -s 0 re15_port_v0.8.19_win64.zip --out win64_ganz.zip \
          && zip -q -s 0 re15_port_v0.8.19_linux_steamdeck_x64.zip --out linux_ganz.zip )
( cd "$W" && unzip -q -o win64_ganz.zip 're15_port_v0.8.19/re15_pc.exe' -d ex_win \
          && unzip -q -o linux_ganz.zip 're15_port_v0.8.19/re15_pc' -d ex_linux )
mkdir -p "$BAUM/release/win_out" "$BAUM/release/linux_out"
[[ ! -e "$BAUM/release/win_out/re15_pc.exe" && ! -e "$BAUM/release/linux_out/re15_pc" ]] \
    || { echo "win_out/linux_out schon belegt - nicht ueberschreiben (anderer Pruefer?)" >&2; exit 3; }
cp -p "$W/ex_win/re15_port_v0.8.19/re15_pc.exe" "$BAUM/release/win_out/re15_pc.exe"
cp -p "$W/ex_linux/re15_port_v0.8.19/re15_pc"   "$BAUM/release/linux_out/re15_pc"
ls -la --time-style=full-iso "$BAUM/release/win_out/re15_pc.exe" "$BAUM/release/linux_out/re15_pc"
sha256sum "$BAUM/release/win_out/re15_pc.exe" "$BAUM/release/linux_out/re15_pc"
echo "letzter PC-Code-Commit (make_package-Standardpfade):"
git -C "$BAUM" log -1 --format='%h %ci %s' -- re15_port/engine re15_port/include re15_port/platform ':(exclude)re15_port/platform/android'
echo "letzter Commit an den APK-Pfaden:"
git -C "$BAUM" log -1 --format='%h %ci %s' -- re15_port/engine re15_port/include re15_port/platform/pc re15_port/platform/android
