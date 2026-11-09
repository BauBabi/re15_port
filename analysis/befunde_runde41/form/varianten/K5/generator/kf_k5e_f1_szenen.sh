#!/usr/bin/env bash
# Einbau K5e (E32): Griff-Szene (Spieler in der Klaue, Clip 27 BB_C_GRIFF @0x80105254) und Rueckstoss-Szene (5090) im echten Spiel,
# Bildfolge des echten Fensters (stufe3/gen/s3_spielfolge.sh: eigene exe-Kopie, gdigrab hwnd=, nur eigene PID, kein AUTOSHOT,
# kein SOFTWARE_RENDER). Ablauf wie integration_r41_birkin_pistole (Debug-Sprung 5090@250, Spieler 500,-9175,2048,0, Laufen in den
# Wagen), ohne Feuer, damit die Form greift; Messhaken RE15_BIRKIN_HP_AT setzt danach HP 0 (Rueckstoss ohne langen Beschuss, STUFE3.md 6).
#   bash kf_k5e_f1_szenen.sh <name> [ENV=WERT ...]     -> ruhe/einbau_spiel/szenen_<name>/bild_NNN.png
#   START/DAUER/FPS (Sekunden/Bilder je s, Vorgabe 4/70/2) und HP_AT (KI-Ticks, Vorgabe 1400) als Umgebung.
#   Gemessen im ersten Lauf (birkin_dbg.log, 2 Bilder/s): Griff Clip 27 bei t=450..540 (dist 7628 -> 10340: Leon fliegt), Fensterbild ~70;
#   Leon stirbt ohne Feuer nach t=1110 (Bild ~104), HP_AT=1400 kam zu spaet. Darum: Griff-Lauf START 34 DAUER 10 FPS 8,
#   Rueckstoss-Lauf HP_AT 700 (nach dem Griff, vor dem Tod).
# name asset: shared_assets/RE2 ohne Umleitung; name k5c: RE15_RE2_ASSET_ROOT=varianten/K5/einbau_k5c (als ENV-Argument uebergeben).
W=$(cd "$(dirname "$0")/../../../../../.." && pwd)
NAME=${1:?name}; shift
export AUS_DIR="$W/analysis/befunde_runde41/form/varianten/K5/ruhe/einbau_spiel/szenen_$NAME"
bash "$W/analysis/befunde_runde41/form/stufe3/gen/s3_spielfolge.sh" "f1_$NAME" 5090@250 "${START:-4}" "${DAUER:-70}" "${FPS:-2}" "A0.1,W1,U8.5" \
    RE15_NOAUDIO=1 "RE15_PLAYER_POS=500,-9175,2048,0" RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 "RE15_BIRKIN_HP_AT=${HP_AT:-1400}" "$@"
