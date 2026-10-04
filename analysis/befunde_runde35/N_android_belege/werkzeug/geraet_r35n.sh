#!/usr/bin/env bash
# Runde 35 Spur N: Emulator-Messung (eigene AVD-Kopie Medium_Phone_API_36_r35n, Port 5600, 2400x1080 quer).
# Aufruf: geraet_r35n.sh <apk> <ausgabeordner>
#   E1 Uebergang v0.8.19: alter Marker re15_assets_ok.txt liegt im Speicherordner -> Titel "... EINMALIG GEPRUEFT"
#      (44 Zeichen, auf 2400x1080 bis Runde 34a abgeschnitten) - Bilder waehrend des Entpackens
#   E2 "zuletzt entpackt" ist ein ORDNER -> fehler_halten mit dem laengsten Text (58 Zeichen) - Bild
#   E3 verwaister Ordner <ziel>.neu (wie R4-2 i1, damals ABBRUCH) + Neustart ohne Liste -> laeuft durch
set -u
APK="$1"; OUT="$2"; mkdir -p "$OUT"
export MSYS_NO_PATHCONV=1
ADB="C:/Users/mjoedicke/AppData/Local/Android/Sdk/platform-tools/adb.exe"
adb() { "$ADB" -s emulator-5600 "$@"; }
D=/storage/emulated/0/Android/data/de.re15.port/files
log() { echo "[$(date +%H:%M:%S)] $*"; }
warte_log() {   # $1 = Muster, $2 = Sekunden
    local i
    for ((i = 0; i < $2; i++)); do
        if adb logcat -d -s re15:V 2>/dev/null | grep -qE "$1"; then return 0; fi
        sleep 1
    done
    return 1
}
bild() { adb exec-out screencap -p > "$OUT/$1.png"; log "Bild $1.png ($(wc -c < "$OUT/$1.png") B)"; }
start_app() { adb logcat -c; adb shell am start -W -n de.re15.port/.RE15Activity > /dev/null; }
stop_app() { adb shell am force-stop de.re15.port; }

adb wait-for-device
for ((i = 0; i < 300; i++)); do [[ "$(adb shell getprop sys.boot_completed 2>/dev/null | tr -d '\r')" == 1 ]] && break; sleep 2; done
log "boot_completed=$(adb shell getprop sys.boot_completed | tr -d '\r'), $(adb shell wm size | tr -d '\r')"
adb shell settings put secure immersive_mode_confirmations confirmed
log "install: $(adb install -r "$APK" 2>&1 | tail -1)"

# E1: Speicherordner anlegen (erst ein Start, dann alles weg bis auf den alten Marker)
start_app; warte_log "Entpacken fertig|ABBRUCH" 240; stop_app
adb shell "rm -rf $D/shared_assets $D/synchro $D/re15_assets_entpackt.txt; echo alt > $D/re15_assets_ok.txt; ls $D"
start_app
for t in 3 6 9 12; do sleep 3; bild "E1_uebergang_${t}s"; done
warte_log "Entpacken fertig|ABBRUCH" 240
adb logcat -d -s re15:V | grep -E "Abgleich|Entpacken fertig|ABBRUCH|Konflikt" > "$OUT/E1_logcat.txt"
stop_app

# E2: "zuletzt entpackt" als Ordner -> laengster Fehlertext
adb shell "rm -f $D/re15_assets_entpackt.txt; mkdir -p $D/re15_assets_entpackt.txt; echo x > $D/re15_assets_entpackt.txt/x"
start_app; warte_log "ABBRUCH" 60; sleep 2; bild "E2_fehler_liste"
adb logcat -d -s re15:V | grep -E "ABBRUCH|nicht loeschbar" > "$OUT/E2_logcat.txt"
stop_app
adb shell "rm -rf $D/re15_assets_entpackt.txt"

# E3: verwaister Ordner auf dem Namen einer Zwischendatei (R4-2 i1) + eine Datei veraendert -> ohne Liste: Waisen-Lauf;
#     danach mit Liste: Ordner <ziel>.neu neu anlegen und die Datei loeschen (Liste bleibt) -> Groessen-Nachlauf entpackt
start_app; warte_log "Entpacken fertig|ABBRUCH" 240; stop_app
adb shell "mkdir -p $D/synchro/STAGE1/room1240/main04.wav.neu; echo rest > $D/synchro/STAGE1/room1240/main04.wav.neu/rest; rm -f $D/synchro/STAGE1/room1240/main04.wav"
start_app; warte_log "Entpacken fertig|ABBRUCH" 120; sleep 1
adb logcat -d -s re15:V | grep -E "Abgleich|Entpacken fertig|ABBRUCH|Konflikt|entpacke" > "$OUT/E3_logcat.txt"
adb shell "ls -la $D/synchro/STAGE1/room1240/ | grep main04" >> "$OUT/E3_logcat.txt"
bild "E3_nach_konflikt"
stop_app
log "fertig"
