#!/bin/bash
# Runde 38 Spur D "zopf" — Messung "Leon unveraendert" und "Elza ohne Zopfglieder = vorher" (Dossier D_zopf.md §4).
# Gleiche Fahrt wie integration_r38_zopf (ROOM1120/1121, (2500,10000) Blick 2048 = Ruecken zur Kamera,
# Eingabe W1,R1.0,W4), Bildserie 20..200 je 30 Bilder (RE15_FRAMEDUMP-Serie, RE15_WINDOW_SCALE=1).
#   leon_vor   Basis-exe (f587d34e), Leon        leon_nach  Zweig-exe, Leon                 -> pixelgleich?
#   elza_vor   Basis-exe, Elza                   elza_aus   Zweig-exe, Elza, RE15_ZOPF_SONDE_AUS=1 -> pixelgleich?
#   elza_nach  Zweig-exe, Elza (mit Zopf)        -> Unterschied nur am Kopf
# Aufruf: mess_vorher_nachher.sh <basis.exe> <zweig.exe> <ausgabeordner>
set -u
VOR="$1"; NACH="$2"; OUT="$3"
mkdir -p "$OUT"
lauf() {   # name exe start [KEY=WERT ...]
    local name="$1" exe="$2" start="$3"; shift 3
    local d="$OUT/$name" st raum
    mkdir -p "$d"; rm -f "$d"/*.ppm "$d"/debug.log "$d"/exit.txt
    if [ "$start" = elza ]; then st="RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1"; raum=1121
    else st="RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2"; raum=1120; fi
    ( cd "$d" && env RE15_NO_INTRO=1 RE15_NOAUDIO=1 $st RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=1120@250 \
        RE15_PLAYER_POS=2500,10000,2048 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=30 \
        RE15_INPUT_SCRIPT=W1,R1.0,W4 "RE15_EXIT_AT=220#$raum" "RE15_FRAMEDUMP=20-200/30:f" "$@" \
        timeout 240 "$exe" > /dev/null 2>&1; echo "exit=$?" > exit.txt )
    echo "$name: $(cat "$d/exit.txt"), $(ls "$d"/f*.ppm 2>/dev/null | wc -l) Bilder, $(grep -c . "$d/debug.log" 2>/dev/null) Logzeilen"
}
lauf leon_vor  "$VOR"  leon
lauf leon_nach "$NACH" leon
lauf elza_vor  "$VOR"  elza
lauf elza_aus  "$NACH" elza RE15_ZOPF_SONDE_AUS=1
lauf elza_nach "$NACH" elza
