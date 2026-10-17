#!/bin/bash
# Runde 38 Spur D "zopf" — Messung 4b (Dossier D_zopf.md §4.4/§4.5): Elzas Zopf beim GEHEN und RENNEN an der exe.
# Ein Lauf je Aufruf (Werkzeug-Regel nach dem Haenger 2026-10-17: Bau und exe-Lauf nie koppeln, jeder Lauf mit timeout).
#
#   origfolge  dieselbe Eingabe wie die Original-Messung orig_zopf_gdb.py (ROOM1121, (150,9550) Blick 2048):
#              stehen 4,5 s | Tank-Drehung R 30 Bilder | stehen 90 | gehen U 40 | stehen 60
#              -> Vergleich Bild fuer Bild mit orig_zopf_frames.txt (vergleich_orig.py)
#   origfolge29 wie origfolge, aber 29 Drehbilder (R0.9667): das Original drehte in seinen 30 "drehung"-Bildern 29-mal
#              (Bild 15 noch Blick 2048, Bild 44 = 4832 = 2048 + 29*96) -> gleiche Endrichtung 4832, gleiche Gehrichtung
#   lauf       origfolge29 und danach Kehrt (R 21 Bilder = 2016), stehen 1 s, rennen X+U 14 Bilder, stehen 3 s
#              (= die Fahrt des Riegels integration_r38_zopf Teil "elza_lauf", test_r38_zopf lauf)
#   rennen     (150,9550) Blick 736 (= 4832 & 0xfff, Laufrichtung der Original-Phase "gehen"): stehen 2 s |
#              rennen X+U 20 Bilder | stehen 3 s | rennen mit Drehung X+U+R 1 s | stehen 3 s
#
# Aufruf: mess_laufen.sh <exe> <ausgabeordner> <origfolge|origfolge29|lauf|rennen>
set -u
EXE="$1"; OUT="$2"; ART="$3"
case "$ART" in
    origfolge) POS="150,9550,2048"; SKRIPT="W4.5,R1.0,W3.0,U1.3333,W2.0"; ENDE=400; DUMP="285-395/10:f" ;;
    origfolge29) POS="150,9550,2048"; SKRIPT="W4.5,R0.9667,W3.0333,U1.3333,W2.0"; ENDE=400; DUMP="285-395/10:f" ;;
    lauf)      POS="150,9550,2048"; SKRIPT="W4.5,R0.9667,W3.0333,U1.3333,W2.0,R0.7,W1.0,XU0.4667,W3.0"; ENDE=560; DUMP="285-555/10:f" ;;
    rennen)    POS="150,9550,736";  SKRIPT="W2,XU0.6667,W3,XUR1.0,W3"; ENDE=420; DUMP="90-400/10:f" ;;
    *) echo "unbekannte Art $ART"; exit 2 ;;
esac
d="$OUT/$ART"
mkdir -p "$d"; rm -f "$d"/*.ppm "$d"/debug.log "$d"/zopf.log "$d"/exit.txt
( cd "$d" && env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 \
    RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=1120@250 "RE15_PLAYER_POS=$POS" RE15_INPUT_SCRIPT_BASIS=spiel \
    RE15_INPUT_SCRIPT_START=30 "RE15_INPUT_SCRIPT=$SKRIPT" "RE15_EXIT_AT=$ENDE#1121" "RE15_FRAMEDUMP=$DUMP" \
    RE15_ZOPF_LOG=zopf.log timeout 300 "$EXE" > /dev/null 2> stderr.log; echo "exit=$?" > exit.txt )
echo "$ART: $(cat "$d/exit.txt"), $(ls "$d"/f*.ppm 2>/dev/null | wc -l) Bilder, $(grep -c . "$d/debug.log" 2>/dev/null) Logzeilen, $(grep -c '^Z' "$d/zopf.log" 2>/dev/null) Zopf-Schritte"
