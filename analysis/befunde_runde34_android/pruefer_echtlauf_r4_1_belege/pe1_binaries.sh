#!/usr/bin/env bash
# Pruefer echtlauf R4-1: PC-Binaries v0.8.19 + Referenz-APK aus dem ARCHIV (nur gelesen) nach build/r34a/pruefer_e1/.
# Split-Saetze kopieren (cp -p), sha256 gegen die Archiv-SUMS, zusammenfuehren (zip -s 0), NUR die Binaries entpacken
# (unzip stellt die im Zip gespeicherte mtime her). KEIN touch. Referenz-APK frisch aus dem Archiv, sha256 gegen
# SHA256SUMS_android.txt des Archivs. Rueckgabe != 0 bei jedem Fehler.
set -euo pipefail
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
ARCH=C:/workspace/Re15Data/re15_packages_archiv/v0.8.19
W=$BAUM/build/r34a/pruefer_e1/bin
ZIP=/c/msys64/usr/bin/zip; UNZIP=/c/msys64/usr/bin/unzip
rm -rf "$W"; mkdir -p "$W/satz" "$W/win" "$W/linux"
for f in re15_port_v0.8.19_win64.z01 re15_port_v0.8.19_win64.zip \
         re15_port_v0.8.19_linux_steamdeck_x64.z01 re15_port_v0.8.19_linux_steamdeck_x64.zip; do
    cp -p "$ARCH/$f" "$W/satz/$f"
done
( cd "$W/satz" && grep -E 'win64|linux_steamdeck' "$ARCH/SHA256SUMS.txt" | tr -d '\r' | sha256sum -c - )
SW=$(cygpath -m "$W/satz")
( cd "$W/satz" && $ZIP -q -s 0 "$SW/re15_port_v0.8.19_win64.zip" --out "$SW/win64_ganz.zip" \
          && $ZIP -q -s 0 "$SW/re15_port_v0.8.19_linux_steamdeck_x64.zip" --out "$SW/linux_ganz.zip" )
( cd "$W/satz" && $UNZIP -q -o "$SW/win64_ganz.zip" 're15_port_v0.8.19/re15_pc.exe' -d "$SW/ex_win" \
          && $UNZIP -q -o "$SW/linux_ganz.zip" 're15_port_v0.8.19/re15_pc' -d "$SW/ex_linux" )
cp -p "$W/satz/ex_win/re15_port_v0.8.19/re15_pc.exe" "$W/win/re15_pc.exe"
cp -p "$W/satz/ex_linux/re15_port_v0.8.19/re15_pc" "$W/linux/re15_pc"
rm -rf "$W/satz"
ls -la --time-style=full-iso "$W/win/re15_pc.exe" "$W/linux/re15_pc"
sha256sum "$W/win/re15_pc.exe" "$W/linux/re15_pc"
cp -p "$ARCH/re15_port_v0.8.19_android.apk" "$BAUM/build/r34a/pruefer_e1/ref_v0.8.19.apk"
a=$(sha256sum "$BAUM/build/r34a/pruefer_e1/ref_v0.8.19.apk" | cut -d' ' -f1)
s=$(tr -d '\r' < "$ARCH/SHA256SUMS_android.txt" | cut -d' ' -f1)
echo "Referenz-APK (frische Kopie): $a"
echo "Archiv-SUMS android:          $s"
[[ "$a" == "$s" ]] && echo "Referenz-APK = Archiv" || { echo "Referenz-APK weicht vom Archiv ab"; exit 3; }
