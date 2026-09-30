#!/usr/bin/env bash
# pe1_abbruch.sh <tag> <apk> <ziel-asset>: deinstallieren, frisch installieren, App starten; AUF DEM GERAET wartet eine
# Shell-Schleife, bis <ziel>.neu existiert, und ruft dann sofort `cmd activity force-stop` (kein adb-Rundlauf
# dazwischen). Danach Zustand festhalten: Prozess, oberste Ebene, Dateizahl, *.neu mit Groesse, Ziel, Listen, logcat.
set -u
B=$(cd "$(dirname "$0")" && pwd)
L=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android/build/r34a/pruefer_e1/logs/geraet; mkdir -p "$L"
A="bash $B/pe1_adb.sh"
TAG="$1"; APK="$2"; ZIEL="$3"
R=/sdcard/Android/data/de.re15.port/files
{
echo "== $TAG $(date '+%F %T') APK $(sha256sum "$APK" | cut -c1-16)... Ausloeser: $ZIEL.neu"
$A uninstall de.re15.port | tr -d '\r'
echo "externer Ordner nach uninstall: $($A shell "ls -d $R 2>&1" | tr -d '\r')"
out=$($A install -r "$APK" 2>&1); echo "install: $(echo "$out" | tr -d '\r' | tr '\n' ' ')"
echo "$out" | grep -q '^Success' || { echo "INSTALL FEHLGESCHLAGEN"; exit 1; }
$A logcat -c
$A shell "i=0; while [ ! -e $R/$ZIEL.neu ]; do i=\$((i+1)); [ \$i -gt 30000000 ] && break; done; cmd activity force-stop de.re15.port; echo AUSLOESER i=\$i; ls -la $R/$ZIEL.neu $R/$ZIEL 2>&1" > "$L/${TAG}_ausloeser.txt" 2>&1 &
AUS=$!
$A shell am start -n de.re15.port/.RE15Activity | tr -d '\r'
wait $AUS
tr -d '\r' < "$L/${TAG}_ausloeser.txt"
echo "Prozess: $($A shell 'pidof de.re15.port || echo beendet' | tr -d '\r')"
echo "--- Zustand nach dem Abbruch:"
$A shell "cd $R && ls -a && echo Dateien: \$(find shared_assets synchro -type f 2>/dev/null | wc -l) && echo '.neu:' && find . -name '*.neu' -exec ls -la {} \; ; ls -la $ZIEL re15_assets_entpackt.txt re15_assets_ok.txt 2>&1" | tr -d '\r'
$A logcat -d -v time -s re15:V > "$L/$TAG.logcat"
grep -v '^---------' "$L/$TAG.logcat" | cut -c1-300
} 2>&1 | tee "$L/$TAG.txt"
