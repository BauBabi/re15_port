#!/usr/bin/env bash
# Nachbesserung R4-1: Emulator-Nachweis des geaenderten Entpackers (APK NB1 = v0.8.20-nb1, Faelschungen aus
# nb_faelschen.sh). Instanz emulator-5584 (eigene AVD-Kopie build/r34a/nb/avd, 12 GB /data).
#   e1  frisch NB1 (ohne Liste, 0 Waisen) + Titelbild          e2  Neustart -> schneller Weg
#   e3  v0.8.19 frisch, Waisen gepflanzt, Update auf NB1 (Uebergang: Waisen entfernt, konsistent)
#   e4  Abbruch mitten in CDEMD0.EMS, Update auf R (ohne CDEMD0.EMS): .neu-Waise entfernt (Pruefer E1 "j")
#   e5  Abbruch in ENEMSE.VBS (CDEMD0.EMS fertig), Update auf R: fertige CDEMD0.EMS als Waise entfernt (Pruefer E1 "k")
#   e6  Kelvin-APK (FK) als Update: Liste ungueltig -> Meldung bleibt stehen, kein Spielstart, Baum unveraendert (H5/U4)
#   e7  NB1 als Update ueber den e6-Zustand: Update-Weg holt CDEMD0.EMS zurueck, konsistent
# Aufruf: nb_emu_folge.sh [schritt ...]   (ohne Argument: alle)
set -u
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
B=build/r34a/nb/emu; A="bash $B/nb_adb.sh"; L=build/r34a/nb/logs/geraet; mkdir -p "$L"
NB1=build/r34a/nb/apk/NB1.apk; REF=build/r34a/ref_v0.8.19.apk; F=build/r34a/nb/faelsch
R=/sdcard/Android/data/de.re15.port/files
SCHRITTE="${*:-e1 e2 e3 e4 e5 e6 e7}"
geraet() { /c/Python310/python $B/nb_geraet.py "$1" > "$L/$2_geraet.txt" 2>&1; grep -v '^  ' "$L/$2_geraet.txt" | tail -6; grep '^  ' "$L/$2_geraet.txt" | head -8; }
shot() { $A exec-out screencap -p > "$L/$1.png"; echo "Bild $1.png ($(wc -c < "$L/$1.png") B, $(date '+%T'))"; }
debuglog() { $A shell "cat $R/debug.log 2>/dev/null" | tr -d '\r' > "$L/$1_debug.log"; echo "debug.log: $(wc -l < "$L/$1_debug.log") Zeilen, letzte:"; tail -3 "$L/$1_debug.log" | cut -c1-240; }
dran() { [[ " $SCHRITTE " == *" $1 "* ]]; }

$A shell settings put secure immersive_mode_confirmations confirmed
if dran e1; then
    echo "######## e1"; $A uninstall de.re15.port > /dev/null 2>&1
    bash $B/nb_lauf.sh e1_frisch_nb1 "$NB1"
    geraet "$NB1" e1
    for t in 1 2 3 4 5 6; do sleep 8; shot "e1_bild_$t"; done
fi
if dran e2; then
    echo "######## e2"; bash $B/nb_lauf.sh e2_neustart
fi
if dran e3; then
    echo "######## e3"; $A uninstall de.re15.port | tr -d '\r'
    bash $B/nb_lauf.sh e3a_ref0819_frisch "$REF"
    $A shell "am force-stop de.re15.port; echo WAISE > $R/shared_assets/PSX/ALT_WAISE.BIN; mkdir -p $R/synchro/STAGE9/room9999 && echo x > $R/synchro/STAGE9/room9999/alt.wav; echo halb > $R/shared_assets/PSX/DATA/TEX.TIM.neu; ls -la $R/shared_assets/PSX/ALT_WAISE.BIN $R/synchro/STAGE9/room9999/alt.wav $R/shared_assets/PSX/DATA/TEX.TIM.neu" | tr -d '\r'
    bash $B/nb_lauf.sh e3b_uebergang_nb1 "$NB1"
    geraet "$NB1" e3
    $A shell "ls -d $R/synchro/STAGE9 2>&1" | tr -d '\r'
fi
if dran e4; then
    echo "######## e4"; bash $B/nb_abbruch.sh e4a_abbruch_cdemd0 "$NB1" shared_assets/RE2/CDEMD0.EMS
    bash $B/nb_lauf.sh e4b_update_R "$F/R_sig.apk"
    geraet "$F/R_sig.apk" e4
    bash $B/nb_lauf.sh e4c_neustart_R
fi
if dran e5; then
    echo "######## e5"; bash $B/nb_abbruch.sh e5a_abbruch_enemse "$NB1" shared_assets/RE2/ENEMSE.VBS
    bash $B/nb_lauf.sh e5b_update_R "$F/R_sig.apk"
    geraet "$F/R_sig.apk" e5
fi
if dran e6; then
    echo "######## e6"; WARTEN=120 bash $B/nb_lauf.sh e6_kelvin "$F/FK_sig.apk"
    sleep 10; shot e6_fehler_nach_10s; echo "Prozess nach 10 s: $($A shell 'pidof de.re15.port || echo beendet' | tr -d '\r')"
    sleep 20; shot e6_fehler_nach_30s; echo "Prozess nach 30 s: $($A shell 'pidof de.re15.port || echo beendet' | tr -d '\r')"
    $A logcat -d -v time -s re15:V > "$L/e6_kelvin_nach30s.logcat"; echo "logcat re15 nach 30 s: $(grep -vc '^---------' "$L/e6_kelvin_nach30s.logcat") Zeilen"
    grep -v '^---------' "$L/e6_kelvin_nach30s.logcat" | cut -c1-240
    debuglog e6
    echo "K.bin/Kelvin im Baum: $($A shell "ls $R/shared_assets/PSX/ | grep -ci '\.bin$'" | tr -d '\r')"
    geraet "$F/R_sig.apk" e6
    $A shell am force-stop de.re15.port
fi
if dran e7; then
    echo "######## e7"; bash $B/nb_lauf.sh e7_update_nb1 "$NB1"
    geraet "$NB1" e7
    sleep 30; shot e7_bild
fi
echo "FOLGE-ENDE $(date '+%T')"
