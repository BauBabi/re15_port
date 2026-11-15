#!/usr/bin/env bash
# K5g3 Zielband-Lauf (Messblatt K5F 1.1, Verfahren unveraendert): eigene exe-Kopie, echter Renderer (kein AUTOSHOT/SOFTWARE_RENDER),
# RE15_FRAMEDUMP = SDL_RenderReadPixels des eigenen Fensters, Ende per RE15_EXIT_AT bzw. nur ueber die eigene PID (timeout).
# Unterschied zu k5f/skripte/lauf.sh: Ausgabe unter scratchpad k5g2/k5g3tex/spiel, und RE15_FORCE_CUT=<schnitt> muss mitgegeben werden
# (seit der Kamerahalte-Regel engine/src/kamerahalt_r41.c zeichnet RE15_BB_POSE mit Schnitt >= 0 sonst nichts; K5g3/GEOMETRIE.md 5).
#   bash kf_k5g3_zlauf.sh <name> <raum> <von-bis/schritt> [ENV=WERT ...]  -> $S/spiel/<name>/dump_NNNNNN.ppm
set -u
S=${K5G3_SPIEL:-/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/k5g2/k5g3tex}
BAUM=$(cd "$(dirname "$0")/../../../../../.." && pwd)
NAME=$1; RAUM=$2; SERIE=$3; shift 3
ENDE=$(echo "$SERIE" | sed 's/.*-\([0-9]*\).*/\1/')
ID="k5g3z_${NAME}_$$"
LAUF="$BAUM/re15_port/build/form_spiel_k5g3/$ID"
mkdir -p "$LAUF" "$S/spiel/$NAME"
rm -f "$S/spiel/$NAME"/dump_*.ppm
cp "${RE15_PC_EXE:-$BAUM/re15_port/build/platform/pc/re15_pc.exe}" "$LAUF/re15_pc_$ID.exe"
cd "$LAUF"
export SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_WINDOW_SCALE=2 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2
export RE15_DEBUG_JUMP="$RAUM@250" RE15_SPAWN_DIAG=1 RE15_FRAMEDUMP="$SERIE:dump_" RE15_EXIT_AT="$((ENDE + 20))#$RAUM"
for kv in "$@"; do export "$kv"; done
# Fenstergroesse: gelegentlich veraendert der Desktop das Fenster (gemessen 2026-11-14: Abzug 958x1000 statt 640x480); dann bis zu
# dreimal neu laufen - nur Abzuege in 640x480 sind mit dem Messblatt vergleichbar.
for versuch in 1 2 3; do
  rm -f dump_*.ppm debug.log
  timeout 240 "./re15_pc_$ID.exe" >exe_stdout.log 2>exe_stderr.log
  RC=$?
  grep -aq "\[framedump\] 640x480" debug.log && ! grep -a "\[framedump\]" debug.log | grep -vq "640x480" && break
  echo "Versuch $versuch: Abzug nicht 640x480 ($(grep -a framedump debug.log | head -1)), neu"
done
echo "rc=$RC $(ls dump_*.ppm 2>/dev/null | wc -l) Bilder"
cp dump_*.ppm "$S/spiel/$NAME/" 2>/dev/null
cp exe_stderr.log "$S/spiel/$NAME/stderr.log"; cp debug.log "$S/spiel/$NAME/debug.log" 2>/dev/null
grep -a "Bahnsteig-Form\|tri-liste\] ENDE" debug.log 2>/dev/null | sort -u | head -4
cd "$BAUM/re15_port/build/form_spiel_k5g3" && rm -rf "$ID"
