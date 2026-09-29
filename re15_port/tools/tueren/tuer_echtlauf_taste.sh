#!/usr/bin/env bash
# tuer_echtlauf_taste.sh - ECHTLAUF der echten exe durch eine Tuer PER AKTIONSTASTE (Runde 33, Stufe 2).
# Dossier: analysis/befunde_runde33/tueren_rest_bau.md. Vorbild tuer_echtlauf.sh (Runde 31), aber statt
# RE15_FIRE_AOT steht der Spieler VOR der Tuer (RE15_PLAYER_POS="x,z,rot,band", Standplatz aus
# probe_r33_tueren standplatz <raum> <ziel> [bank:bit]) und drueckt QUADRAT (RE15_PRESS=square@<bild>) -
# der normale Spielschritt (AOT-Pruefung, Door_aot_set-Nutzlast, Laeufer, Raumwechsel).
#
# Weg: Vorspann/Titel (RE15_TITLE_SHOT Auto-Vorlauf) -> RE15_DEBUG_JUMP=<raum>@120 (Original-UTILITY-MENU,
# Spawn ersetzt durch RE15_PLAYER_POS) -> QUADRAT am Bild <druck> des Raums -> Tuersequenz -> Zielraum.
# Bilder (beschleunigter Renderer, Rueckleser vor dem Present; kein AUTOSHOT/SOFTWARE_RENDER):
#   <ziel>/f_NNNNNN.ppm   RE15_FRAMEDUMP (alter Raum bis zum Druck; Bilder 0..60 = neuer Raum, Einblendung -
#                          g_engine.frame_count beginnt beim Raumwechsel wieder bei 0, s. tuer_echtlauf.sh)
#   <ziel>/serie/*.ppm    RE15_TUER_SERIE (a_abdunkeln_*, b_tuer_*, c_ende_*)
# Ton: RE15_AUDIO_CAP_SYNC=<ziel>/ton.pcm, RE15_SE_DEBUG=1 (debug.log: "[se]"/"[tuer] Bild N Se_on").
#
# Aufruf: tuer_echtlauf_taste.sh <ziel> <raum-hex> <x,z,rot,band> <zielraum-hex> [druck=300] [sekunden=240] [env ...]
set -uo pipefail
HIER="$(cd "$(dirname "$0")" && pwd)"
BAUM="$(cd "$HIER/../../.." && pwd)"
ZIEL="${1:?Zielverzeichnis fehlt}"
RAUM="${2:?Raum fehlt}"
POS="${3:?Standplatz fehlt}"
ZRAUM="${4:?Zielraum fehlt}"
DRUCK="${5:-300}"
SEK="${6:-240}"
shift 6 2>/dev/null || shift $#
ORG="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
EXE="$BAUM/re15_port/build/platform/pc/re15_pc_r33e.exe"     # eigener Name: andere Agenten beenden re15_pc.exe
cp -f "$ORG" "$EXE"
mkdir -p "$ZIEL/serie"
ZIEL="$(cd "$ZIEL" && pwd)"
rm -f "$ZIEL"/debug.log "$ZIEL"/f_*.ppm "$ZIEL"/serie/*.ppm "$ZIEL"/ton.pcm

export RE15_NO_INTRO=1
export RE15_TITLE_SHOT="$ZIEL/titel.bmp"
export RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="${RAUM}@${JUMP_BILD:-120}"
export RE15_PLAYER_POS="$POS"
export RE15_PRESS="square@${DRUCK}"
export RE15_TUER_SERIE="$ZIEL/serie"
export RE15_FRAMEDUMP="0-${DRUCK}/2:$ZIEL/f_"
export RE15_EXIT_AT="60#${ZRAUM}"
export RE15_AUDIO_CAP_SYNC="$ZIEL/ton.pcm"
export RE15_SE_DEBUG=1
for kv in "$@"; do export "$kv"; done
cd "$(dirname "$EXE")"
timeout -k 5 "$SEK" "$EXE" > "$ZIEL/stdout.txt" 2> "$ZIEL/stderr.txt"
echo "exit=$?"
cp -f "$(dirname "$EXE")/debug.log" "$ZIEL/debug.log" 2>/dev/null
grep -a "JUMP\|parity\|press\|\[tuer\]\|\[room\]\|room_apply\|EXIT_AT\|\[se\]\|setflag" "$ZIEL/debug.log" 2>/dev/null | head -60
for f in "$ZIEL"/f_*.ppm; do n=$(basename "$f" .ppm); n=$((10#${n#f_}))
    if [ "$n" -gt 62 ] && [ "$n" -lt $((DRUCK - 12)) ]; then rm -f "$f"; fi; done
ls "$ZIEL"/f_*.ppm 2>/dev/null | wc -l
ls "$ZIEL"/serie 2>/dev/null | wc -l
