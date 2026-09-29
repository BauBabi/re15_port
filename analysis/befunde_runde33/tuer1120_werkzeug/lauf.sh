#!/usr/bin/env bash
# lauf.sh - Abnahme Runde 33 / Thema R im ECHTEN Spiel (re15_pc.exe, beschleunigter Renderer,
# Bilder per RE15_FRAMEDUMP = komplett komponierter Frame VOR dem Present; KEIN AUTOSHOT,
# KEIN SOFTWARE_RENDER). Die exe wird unter eigenem Namen kopiert (parallele Agenten).
#
#   lauf.sh vorher <ziel>   Neues Spiel -> Debug-JUMP ROOM1150 ausserhalb des Szenen-Platzes (Flag
#                           (3,94) bleibt 0) -> Tuer 1150->1130 (Transit, Ankunft (-5900,0,16350)
#                           yaw 0 = ROOM1150 @0x00D5E) -> er LAEUFT per Eingabeskript (echter
#                           Eingabepfad) zur Tuer nach ROOM1120 und drueckt EINMAL die Aktionstaste.
#   lauf.sh nachher <ziel>  Neues Spiel -> Debug-JUMP ROOM1150, Spieler im AUTO-Platz der Irons-Szene
#                           (main00 @0x00DEA) -> sub08 laeuft echt und setzt (3,94) (@0x01110) ->
#                           Kartenhinweis mit Quadrat schliessen -> Tuer 1150->1130 (RE15_FIRE_AOT =
#                           derselbe aot_fire_door wie das Hineinlaufen; nur der TRANSIT) -> in
#                           ROOM1130 derselbe Weg + dieselbe Aktionstaste -> Tuersequenz -> ROOM1120.
#
# ⛔ Zeitachsen-Falle: RE15_INPUT_SCRIPT_BASIS=spiel, RE15_PRESS und RE15_FRAMEDUMP zaehlen
# g_engine.frame_count, der bei JEDEM Raumwechsel auf 0 faellt. Das Skript fuer ROOM1130 beginnt im
# Nachher-Lauf deshalb erst bei Bild 2500 — ROOM1150 verlaesst der Lauf bei Bild ~1700, erreicht
# 2500 dort also nie. Die Bildserie 2400..3300 gehoert damit allein ROOM1130.
set -uo pipefail
MODUS="${1:?vorher|nachher}"; ZIEL="${2:?Zielverzeichnis}"
HIER="$(cd "$(dirname "$0")" && pwd)"
BAUM="$(cd "$HIER/../../.." && pwd)"
SRC="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
EXE="$BAUM/re15_port/build/platform/pc/re15_pc_r33tuer.exe"
cp -f "$SRC" "$EXE" || exit 2
mkdir -p "$ZIEL"; ZIEL="$(cd "$ZIEL" && pwd)"
rm -f "$ZIEL"/*.ppm "$ZIEL"/debug.log "$ZIEL"/stderr.txt "$ZIEL"/state.log
mkdir -p "$ZIEL/serie"; rm -f "$ZIEL"/serie/*.ppm
# Weg durch ROOM1130 (Kollision @0x598: Westwand x<=-6650, Block x>=-3950 bei z 4200..14700,
# Wand x<=-3050 ab z<=1050; gemessen: R dreht 96/Bild, Gehen 74..75/Bild):
#   Blick 0 -> R 11 Bilder = 1056 (Sued) -> 6,1 s gehen -> L 11 Bilder = 0 (Ost) -> 1,67 s ->
#   R 11 Bilder = 1056 -> 2,1 s -> R 10 Bilder = 2016 (West, Tuer) -> Aktionstaste 3 Bilder.
WEG="R0.3667,W0.3,U6.1,W0.3,L0.3667,W0.3,U1.6667,W0.3,R0.3667,W0.3,U2.1,W0.3,R0.3333,W0.5,A0.1,W8"
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NO_INTRO=1 RE15_NOAUDIO=1
export RE15_TITLE_SHOT=titel.bmp RE15_TITLE_SHOT_AF=2
export RE15_FLAG_TRACE=1 RE15_FLAG_CENSUS=1 RE15_STATE_LOG=state.log
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT="${WEG_ENV:-$WEG}"
# RE15_FRAMEDUMP trennt am LETZTEN Doppelpunkt -> Praefix RELATIV (ein C:/ wuerde zerschnitten).
case "$MODUS" in
vorher)
    # ROOM1150 AUSSERHALB des AUTO-Platzes der Szene betreten (z -13500 > -21800), sofort durch die
    # Tuer nach ROOM1130 (Transit, damit die Kamera-Zonen wie im Spiel laufen — nach einem
    # Debug-JUMP direkt nach ROOM1130 blieb die Kamera gemessen auf Cut 0 stehen), dort derselbe Weg.
    export RE15_DEBUG_JUMP="1150@120"
    export RE15_PLAYER_POS="-16500,-13500,0"
    export RE15_FIRE_AOT="0@300#1150"
    export RE15_INPUT_SCRIPT_START=600
    export RE15_FRAMEDUMP="${SERIE:-600-1400/15}:v_"
    export RE15_EXIT_AT="${ENDE:-1400}#1130"
    ;;
nachher)
    export RE15_DEBUG_JUMP="1150@120"
    export RE15_PLAYER_POS="-20500,-22800,1024"          # im AUTO-Platz Slot 6 (-27300..-15800, -23800..-21800)
    export RE15_PAD_AT="1600:X"                           # Kartenhinweis zu: Abbruch-Flanke (menu_common.c Hinweis-Zweig, RE2 Maske 0x6000 @0x8006F884)
    export RE15_FIRE_AOT="0@1700#1150"                    # Transit ROOM1150 -> ROOM1130
    export RE15_INPUT_SCRIPT_START=2500
    export RE15_TUER_SERIE=serie
    export RE15_FRAMEDUMP="${SERIE:-2400-3300/15}:n_"
    export RE15_EXIT_AT="${ENDE:-60}#1120"
    ;;
*) echo "Modus?"; exit 2;;
esac
cd "$ZIEL"
timeout -k 5 "${SEK:-400}" "$EXE" > "$ZIEL/stdout.txt" 2> "$ZIEL/stderr.txt"
echo "exit=$?"
grep -a -E "AUTO-JUMP|JUMP ->|tuer1120|fire-aot|room\] PC loaded|EXIT_AT|WRITE Set  zone=3 idx=94|\[hint\] F[0-9]+ begin|\[tuer\] Sequenz|\[press\]|input-script\] Tick" "$ZIEL/debug.log" 2>/dev/null | head -40
ls "$ZIEL"/*.ppm 2>/dev/null | wc -l
