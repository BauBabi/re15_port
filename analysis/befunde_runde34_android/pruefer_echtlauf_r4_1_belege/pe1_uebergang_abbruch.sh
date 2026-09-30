#!/usr/bin/env bash
# pe1_uebergang_abbruch.sh: Uebergang v0.8.19 -> M (M traegt EINE gleich grosse Aenderung, die der alte Entpacker nie
# saehe), mit force-stop mitten in der Pruefphase und Neustart. Schritte:
#   g1 deinstallieren, Referenz v0.8.19 frisch installieren + entpacken (alter Entpacker)
#   g2 M als Update, starten, nach "Abgleich (Uebergang" ~6 s warten, force-stop; Zustand festhalten
#   g3 Neustart -> erwartet erneut "Uebergang v0.8.19" (Marker noch da) und "Summe weicht ab -> neu: ...ROOM4010.RDT"
set -u
B=$(cd "$(dirname "$0")" && pwd)
W=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android/build/r34a/pruefer_e1
L=$W/logs/geraet
A="bash $B/pe1_adb.sh"
R=/sdcard/Android/data/de.re15.port/files
$A uninstall de.re15.port | tr -d '\r'
WARTEN=600 bash "$B/pe1_lauf.sh" g1_ref0819_frisch "$W/ref_v0.8.19.apk" || exit 1
{
echo "== g2_uebergang_abbruch $(date '+%F %T')"
out=$($A install -r "$W/apk/M_v0.8.19-pe1m.apk" 2>&1); echo "install M: $(echo "$out" | tr -d '\r' | tr '\n' ' ')"
echo "$out" | grep -q '^Success' || { echo "INSTALL FEHLGESCHLAGEN"; exit 1; }
$A shell am force-stop de.re15.port
$A logcat -c
$A shell am start -n de.re15.port/.RE15Activity | tr -d '\r'
timeout 300 bash "$B/pe1_adb.sh" logcat -v time -s re15:V -e "Abgleich \(Uebergang" -m 1 | tr -d '\r' | grep -v '^---------'
ping -n 7 127.0.0.1 > /dev/null
$A shell cmd activity force-stop de.re15.port
echo "force-stop $(date '+%T'); Prozess: $($A shell 'pidof de.re15.port || echo beendet' | tr -d '\r')"
$A shell "cd $R && ls -a && echo '.neu:' && find . -name '*.neu' -exec ls -la {} \; ; cat re15_assets_ok.txt; echo; sha256sum shared_assets/PSX/STAGE4/ROOM4010.RDT; ls -la re15_assets_entpackt.txt 2>&1" | tr -d '\r'
$A logcat -d -v time -s re15:V > "$L/g2_uebergang_abbruch.logcat"
grep -v '^---------' "$L/g2_uebergang_abbruch.logcat" | cut -c1-300
} 2>&1 | tee "$L/g2_uebergang_abbruch.txt"
WARTEN=600 bash "$B/pe1_lauf.sh" g3_neustart_nach_uebergang_abbruch
{
rm -f "$L/g3_pull_ROOM4010.RDT"
$A pull "$R/shared_assets/PSX/STAGE4/ROOM4010.RDT" "$(cygpath -m "$L")/g3_pull_ROOM4010.RDT" | tr -d '\r'
echo "adb pull sha256: $(sha256sum "$L/g3_pull_ROOM4010.RDT" | cut -c1-64)"
cmp "$L/g3_pull_ROOM4010.RDT" "$L/c_M_ROOM4010.RDT" && echo "cmp pull == Inhalt in APK M: rc 0"
/c/Python310/python.exe "$B/pe1_geraet.py" "$W/apk/M_v0.8.19-pe1m.apk"
} 2>&1 | tee "$L/g3_geraet.txt"
