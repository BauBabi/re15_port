#!/usr/bin/env bash
# pe2_abbruch.sh <tag> <relpfad> [<apk_frisch>|""] [<schwelle_bytes>=1048576] - optional deinstallieren + APK frisch installieren; eine Schleife AUF dem
# Geraet wartet, bis <relpfad>.neu >= <schwelle> B ist, und ruft sofort "am force-stop"; danach Zustand festhalten.
set -u
B=$(cd "$(dirname "$0")" && pwd)
L=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android/build/r34a/pruefer_e2/logs/geraet; mkdir -p "$L"
A="bash $B/pe2_adb.sh"
R=/sdcard/Android/data/de.re15.port/files
TAG="$1"; REL="$2"; APK="${3:-}"; SCHWELLE="${4:-1048576}"
{
echo "== $TAG $(date '+%F %T') Ziel: $REL.neu >= $SCHWELLE B -> force-stop"
if [[ -n "$APK" ]]; then
    echo "uninstall: $($A uninstall de.re15.port 2>&1 | tr -d '\r')"
    echo "Speicherordner nach uninstall: $($A shell "ls -d $R 2>&1" | tr -d '\r')"
    out=$($A install "$APK" 2>&1); echo "install: $(echo "$out" | tr -d '\r' | tr '\n' ' ')"
    echo "$out" | tr -d '\r' | grep -qx 'Success' || { echo "INSTALL FEHLGESCHLAGEN"; exit 1; }
    echo "installiert: $($A shell pm path de.re15.port | tr -d '\r') $($A shell dumpsys package de.re15.port | tr -d '\r' | grep -m1 versionName)"
fi
$A shell am force-stop de.re15.port
$A logcat -c
$A shell "while [ \$(stat -c %s '$R/$REL.neu' 2>/dev/null || echo 0) -lt $SCHWELLE ]; do :; done; am force-stop de.re15.port; echo WAECHTER: force-stop bei \$(stat -c %s '$R/$REL.neu' 2>/dev/null) B" > "$L/$TAG.waechter" 2>&1 &
wp=$!
t1=$(date +%s)
$A shell am start -n de.re15.port/.RE15Activity | tr -d '\r'
wait $wp
echo "Waechter: $(cat "$L/$TAG.waechter" | tr -d '\r') ($(( $(date +%s) - t1 )) s nach am start)"
sleep 2
echo "pidof nach Abbruch: [$($A shell pidof de.re15.port | tr -d '\r')]"
$A logcat -d -v time -s re15:V > "$L/$TAG.logcat"
grep -v '^---------' "$L/$TAG.logcat" | cut -c1-300
echo "Zustand nach Abbruch:"
$A shell "cd $R && ls -a; echo; ls -la '$REL' '$REL.neu' 2>&1; echo; echo Dateien: \$(find shared_assets synchro -type f 2>/dev/null | wc -l); echo neu-Dateien:; find . -name '*.neu' -exec ls -la {} \;" | tr -d '\r' | sed 's/^/   /'
} 2>&1 | tee "$L/$TAG.txt"
