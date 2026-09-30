#!/usr/bin/env bash
# Runde 4 (Kette): PC-Binaries v0.8.19 aus dem ARCHIV (nur gelesen) nach build/r34a/r4/bin/{win,linux}/ holen -
# wie pruefer_echtlauf_r3_belege/binaries_vorbereiten_r3.sh: Split-Saetze kopieren, sha256 gegen die Archiv-SUMS,
# zusammenfuehren (zip -s 0), NUR die Binaries entpacken (unzip stellt die Original-mtime her). KEIN touch.
# Die Referenz-APK build/r34a/ref_v0.8.19.apk wird gegen die Archiv-SUMS geprueft. Rueckgabe != 0 bei jedem Fehler.
set -euo pipefail
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
ARCH=C:/workspace/Re15Data/re15_packages_archiv/v0.8.19
W=$BAUM/build/r34a/r4/bin
ZIP=/c/msys64/usr/bin/zip; UNZIP=/c/msys64/usr/bin/unzip
rm -rf "$W"; mkdir -p "$W/satz"
for f in re15_port_v0.8.19_win64.z01 re15_port_v0.8.19_win64.zip \
         re15_port_v0.8.19_linux_steamdeck_x64.z01 re15_port_v0.8.19_linux_steamdeck_x64.zip; do
    cp -p "$ARCH/$f" "$W/satz/$f"
done
( cd "$W/satz" && grep -E 'win64|linux_steamdeck' "$ARCH/SHA256SUMS.txt" | sha256sum -c - )
( cd "$W/satz" && $ZIP -q -s 0 re15_port_v0.8.19_win64.zip --out win64_ganz.zip \
          && $ZIP -q -s 0 re15_port_v0.8.19_linux_steamdeck_x64.zip --out linux_ganz.zip )
( cd "$W/satz" && $UNZIP -q -o win64_ganz.zip 're15_port_v0.8.19/re15_pc.exe' -d ex_win \
          && $UNZIP -q -o linux_ganz.zip 're15_port_v0.8.19/re15_pc' -d ex_linux )
mkdir -p "$W/win" "$W/linux"
cp -p "$W/satz/ex_win/re15_port_v0.8.19/re15_pc.exe" "$W/win/re15_pc.exe"
cp -p "$W/satz/ex_linux/re15_port_v0.8.19/re15_pc" "$W/linux/re15_pc"
rm -rf "$W/satz"
ls -la --time-style=full-iso "$W/win/re15_pc.exe" "$W/linux/re15_pc"
sha256sum "$W/win/re15_pc.exe" "$W/linux/re15_pc"
echo "Referenz-APK: $(sha256sum "$BAUM/build/r34a/ref_v0.8.19.apk" | cut -d' ' -f1)"
echo "Archiv-SUMS:  $(cut -d' ' -f1 "$ARCH/SHA256SUMS_android.txt")"
[[ "$(sha256sum "$BAUM/build/r34a/ref_v0.8.19.apk" | cut -d' ' -f1)" == "$(cut -d' ' -f1 "$ARCH/SHA256SUMS_android.txt")" ]] \
    && echo "Referenz-APK = Archiv" || { echo "Referenz-APK weicht vom Archiv ab"; exit 3; }
