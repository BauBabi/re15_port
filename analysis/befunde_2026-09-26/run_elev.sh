#!/bin/bash
# Messlauf-Treiber Fahrstuhl ROOM1080 (MESSUNG, kein Fix).
# $1 = Ausgabe-Praefix, $2 = RE15_PRESS-Spec (darf leer sein), $3 = Laufzeit s
set -u
PFX="${1:-lauf}"
PRESS="${2:-}"
SECS="${3:-70}"
OUT="analysis/befunde_2026-09-26/mess"
mkdir -p "$OUT"
export RE15_NOAUDIO=1
export RE15_TITLE_SHOT="$OUT/title.png"
export RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="${RE15_JUMPSPEC:-1080@gp}"
export RE15_PLAYER_POS="-12050,-670,3072"
export RE15_ELEV_LOG="$OUT/${PFX}.log"
if [ -n "$PRESS" ]; then export RE15_PRESS="$PRESS"; fi
timeout "$SECS" ./re15_port/build/platform/pc/re15_pc.exe > "$OUT/${PFX}.out" 2> "$OUT/${PFX}.err"
echo "rc=$?"
echo "Zeilen im Messprotokoll: $(wc -l < "$OUT/${PFX}.log")"
