#!/usr/bin/env bash
# lauf.sh - Abnahme Runde 33 / Thema R im ECHTEN Spiel (re15_pc.exe, beschleunigter Renderer,
# Bilder per RE15_FRAMEDUMP = komplett komponierter Frame VOR dem Present; KEIN AUTOSHOT,
# KEIN SOFTWARE_RENDER). Die exe wird unter eigenem Namen kopiert (parallele Agenten).
#
#   lauf.sh vorher <ziel>   Neues Spiel -> Debug-JUMP ROOM1130 (Flag (3,94) = 0) -> vor die Tuer
#                           nach ROOM1120 -> EIN Quadrat-Druck (RE15_PRESS, echter Aktions-Scan).
#   lauf.sh nachher <ziel>  Neues Spiel -> Debug-JUMP ROOM1150, Spieler im AUTO-Platz der Irons-
#                           Szene (sub08 laeuft echt, setzt (3,94)) -> Tuer 1150->1130 (FIRE_AOT,
#                           Transit) -> in ROOM1130 der Autopilot (nur Pad-Bits) zur Tuer Slot 1,
#                           Aktionstaste -> Tuersequenz -> ROOM1120.
# Zusatz-Umgebung wird durchgereicht (z.B. SERIE=..., FEUER_1150=...).
set -uo pipefail
MODUS="${1:?vorher|nachher}"; ZIEL="${2:?Zielverzeichnis}"
HIER="$(cd "$(dirname "$0")" && pwd)"
BAUM="$(cd "$HIER/../../.." && pwd)"
SRC="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
EXE="$BAUM/re15_port/build/platform/pc/re15_pc_r33tuer.exe"
cp -f "$SRC" "$EXE" || exit 2
mkdir -p "$ZIEL"; ZIEL="$(cd "$ZIEL" && pwd)"
rm -f "$ZIEL"/*.ppm "$ZIEL"/debug.log "$ZIEL"/stderr.txt
mkdir -p "$ZIEL/serie"; rm -f "$ZIEL"/serie/*.ppm
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NO_INTRO=1 RE15_NOAUDIO=1
export RE15_TITLE_SHOT="$ZIEL/titel.bmp" RE15_TITLE_SHOT_AF=2
export RE15_FLAG_TRACE=1 RE15_FLAG_CENSUS=1
case "$MODUS" in
vorher)
    export RE15_DEBUG_JUMP="1130@120"
    export RE15_PLAYER_POS="${POS:--2477,-1914,1792}"      # Standplatz aus test_r33_tuer1120
    export RE15_PRESS="square@${DRUCK:-200}"
    export RE15_FRAMEDUMP="${SERIE:-150-420/10}:$ZIEL/v_"
    export RE15_EXIT_AT="${ENDE:-430}#1130"
    ;;
nachher)
    export RE15_DEBUG_JUMP="1150@120"
    export RE15_PLAYER_POS="${POS:--20500,-22800,1024}"     # im AUTO-Platz Slot 6 (-27300..-15800, -23800..-21800)
    export RE15_FIRE_AOT="0@${FEUER_1150:-2400}#1150"
    export RE15_AUTOPILOT_ROOM=1130
    export RE15_AUTOPILOT="${AP:-xz:-5250,12000;xz:-5000,3000;xz:-1650,-1000;aot:1}"   # Nav-Zonen 0->1->2->3 (blk @0x848)
    export RE15_TUER_SERIE="$ZIEL/serie"
    export RE15_FRAMEDUMP="${SERIE:-0-3000/30}:$ZIEL/n_"
    export RE15_EXIT_AT="${ENDE:-90}#1120"
    ;;
*) echo "Modus?"; exit 2;;
esac
cd "$ZIEL"
timeout -k 5 "${SEK:-400}" "$EXE" > "$ZIEL/stdout.txt" 2> "$ZIEL/stderr.txt"
echo "exit=$?"
grep -a -E "AUTO-JUMP|JUMP ->|tuer1120|fire-aot|\[auto\] Autopilot|room\] PC loaded|EXIT_AT|WRITE Set  zone=3 idx=94|\[hint\] F[0-9]+ begin|\[tuer\] Sequenz|\[press\]" "$ZIEL/debug.log" 2>/dev/null | head -40
ls "$ZIEL"/*.ppm 2>/dev/null | wc -l
