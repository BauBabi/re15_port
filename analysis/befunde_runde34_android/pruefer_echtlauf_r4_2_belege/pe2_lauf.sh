#!/usr/bin/env bash
# pe2_lauf.sh <tag> [<apk>] - optional "adb install -r" (Erfolg nur mit Zeile "Success"), installierte base.apk per
# sha256 benennen, App kalt starten, auf die Abschlusszeile des Entpackers warten, logcat (Tag re15) sichern.
set -u
B=$(cd "$(dirname "$0")" && pwd)
L=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android/build/r34a/pruefer_e2/logs/geraet; mkdir -p "$L"
A="bash $B/pe2_adb.sh"
TAG="$1"; APK="${2:-}"
ENDE='Entpacken fertig|Assets aktuell|ABBRUCH'
{
echo "== $TAG $(date '+%F %T')"
if [[ -n "$APK" ]]; then
    echo "APK: $APK sha256 $(sha256sum "$APK" | cut -c1-64)"
    t0=$(date +%s%N)
    out=$($A install -r "$APK" 2>&1); rc=$?
    echo "install -r rc=$rc ($(( ($(date +%s%N) - t0) / 1000000 )) ms): $(echo "$out" | tr -d '\r' | tr '\n' ' ')"
    echo "$out" | tr -d '\r' | grep -qx 'Success' || { echo "INSTALL FEHLGESCHLAGEN"; exit 1; }
fi
$A shell dumpsys package de.re15.port | tr -d '\r' | grep -E "versionCode|versionName|lastUpdateTime|firstInstallTime" | sort -u | sed 's/^ */   /'
p=$($A shell pm path de.re15.port | tr -d '\r' | sed 's/^package://')
echo "installiert: $p sha256 $($A shell sha256sum "$p" | cut -c1-64)"
$A shell am force-stop de.re15.port
$A logcat -c
t1=$(date +%s%N)
$A shell am start -W -n de.re15.port/.RE15Activity | tr -d '\r' | grep -E "Status|LaunchState|TotalTime"
timeout "${WARTEN:-900}" bash "$B/pe2_adb.sh" logcat -v time -s re15:V -e "$ENDE" -m 1 > /dev/null
echo "warten rc=$? ($(( ($(date +%s%N) - t1) / 1000000 )) ms ab am start)"
sleep 1
$A logcat -d -v time -s re15:V > "$L/$TAG.logcat"
grep -v '^---------' "$L/$TAG.logcat" | cut -c1-420
echo "pidof: $($A shell pidof de.re15.port | tr -d '\r')"
} 2>&1 | tee "$L/$TAG.txt"
