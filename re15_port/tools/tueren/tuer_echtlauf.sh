#!/usr/bin/env bash
# tuer_echtlauf.sh - ECHTLAUF der echten re15_pc.exe durch eine Tuer (Runde 31, Stufe 4, Abschnitt 9).
# Dossier: analysis/befunde_runde31/tueren_04_bau.md
#
# Weg: Vorspann/Titel durchklicken (RE15_TITLE_SHOT Auto-Vorlauf) -> RE15_DEBUG_JUMP=<raum>@gp
# (Original-UTILITY-MENU-Pfad; Bild JUMP_BILD, Standard 120 wie r30_5080_echtlauf.sh) -> RE15_FIRE_AOT=<slot>@<bild>#<raum>
# (re15_aot_fire_slot = derselbe aot_fire_door wie das Hineinlaufen) -> die Tuersequenz laeuft vor
# re15_room_apply_pending, danach laedt der Zielraum und blendet ein.
# Bilder (beschleunigter Renderer, Ruecklesen vor dem Present):
#   <ziel>/f_NNNNNN.ppm    RE15_FRAMEDUMP-Serie um das Feuerbild (alter Raum davor, neuer Raum danach)
#   <ziel>/serie/*.ppm     RE15_TUER_SERIE: a_abdunkeln_* (stehendes Bild des alten Raums),
#                          b_tuer_* (Sequenz), c_ende_* (Schwarz bis Kanal 0 fertig)
# Ton ohne Audiogeraet: RE15_AUDIO_CAP_SYNC=<ziel>/ton.pcm + RE15_SE_DEBUG=1 (debug.log).
#
# Aufruf: tuer_echtlauf.sh <ziel> <raum-hex> <slot> <zielraum-hex> [feuerbild=500] [sekunden=180] [zusatz-env ...]
#
# FALLE (gemessen): g_engine.frame_count faengt beim Raumwechsel wieder bei 0 an (main.c
# "Frame-Cap des Handoffs relativ zum Eintritt"). Eine RE15_FRAMEDUMP-Serie um das Feuerbild wird
# deshalb vom neuen Raum UEBERSCHRIEBEN, sobald der die Bildnummern erreicht. Darum: Serie 0..Feuerbild
# und Ende am Bild 60 des ZIELRAUMS - die Bilder 0..60 gehoeren dann dem neuen Raum (Einblendung),
# die Bilder davor bis zum Feuerbild dem alten; das Dazwischen wird hinterher geloescht.
set -uo pipefail
HIER="$(cd "$(dirname "$0")" && pwd)"
BAUM="$(cd "$HIER/../../.." && pwd)"
ZIEL="${1:?Zielverzeichnis fehlt}"
RAUM="${2:?Raum fehlt}"
SLOT="${3:?Slot fehlt}"
ZRAUM="${4:?Zielraum fehlt}"
BILD="${5:-500}"
SEK="${6:-180}"
shift 6 2>/dev/null || shift $#
EXE="${RE15_EXE:-$BAUM/re15_port/build/platform/pc/re15_pc.exe}"
mkdir -p "$ZIEL/serie"
ZIEL="$(cd "$ZIEL" && pwd)"
rm -f "$ZIEL"/debug.log "$ZIEL"/f_*.ppm "$ZIEL"/serie/*.ppm "$ZIEL"/ton.pcm

export RE15_NO_INTRO=1
export RE15_TITLE_SHOT="$ZIEL/titel.bmp"
export RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="${RAUM}@${JUMP_BILD:-120}"
export RE15_FIRE_AOT="${SLOT}@${BILD}#${RAUM}"
export RE15_TUER_SERIE="$ZIEL/serie"
export RE15_FRAMEDUMP="0-${BILD}/2:$ZIEL/f_"
export RE15_EXIT_AT="60#${ZRAUM}"
export RE15_AUDIO_CAP_SYNC="$ZIEL/ton.pcm"
export RE15_SE_DEBUG=1
for kv in "$@"; do export "$kv"; done
cd "$ZIEL"
timeout -k 5 "$SEK" "$EXE" > "$ZIEL/stdout.txt" 2> "$ZIEL/stderr.txt"
echo "exit=$?"
grep -a "AUTO-JUMP\|fire-aot\|\[tuer\]\|\[room\]\|room_apply\|TUER-EINTRITTS\|EXIT_AT\|\[se\] Stimme" "$ZIEL/debug.log" 2>/dev/null | head -40
# Zwischenbilder des alten Raums (62 .. Feuerbild-10) wegwerfen
for f in "$ZIEL"/f_*.ppm; do n=$(basename "$f" .ppm); n=$((10#${n#f_}))
    if [ "$n" -gt 62 ] && [ "$n" -lt $((BILD - 10)) ]; then rm -f "$f"; fi; done
ls "$ZIEL"/f_*.ppm 2>/dev/null | wc -l
ls "$ZIEL"/serie 2>/dev/null | wc -l
