#!/bin/bash
# Messung Spur M: Schluessel an der Pinnwand 10F0 (Leon) / 10F1 (Elza), Cut 5, Bild 150, Skala 1/3/4, mit/ohne Taken-Bit.
# Aufruf: mess.sh <exe-kopie> <ausgabeordner> [skalen...]
EXE="$1"; OUT="$2"; shift 2
SKALEN="${*:-1 3 4}"
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r39_schluessel2"
PROBE="$BAUM/re15_port/build/tests/unit/probe_r38_itemorte_kontrast.exe"
mkdir -p "$OUT"
for v in $(env | grep -o '^RE15_[A-Za-z0-9_]*'); do unset "$v"; done
lauf() {   # name skala env...
  local name="$1" skala="$2"; shift 2
  local d="$OUT/$name"; rm -rf "$d"; mkdir -p "$d"
  ( cd "$d" && env RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=$skala RE15_FRAMEDUMP=150:f.ppm "$@" \
      timeout 240 "$EXE" > stdout.txt 2>&1; echo "exit=$?" > rc.txt )
  [ -f "$d/f.ppm" ] || echo "FEHLT f.ppm: $name ($(cat $d/rc.txt))"
}
LEON="RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=10F0@250 RE15_PLAYER_POS=0,4000,1024 RE15_FORCE_CUT=5 RE15_EXIT_AT=150#10F0"
ELZA="RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1 RE15_GOTO_ROOM=10F1 RE15_PLAYER_POS=0,4000,1024 RE15_FORCE_CUT=5 RE15_EXIT_AT=150#10F1"
for s in $SKALEN; do
  lauf leon_s${s}_mit  $s $LEON RE15_SET_FLAG=9:71,3:94
  lauf leon_s${s}_ohne $s $LEON RE15_SET_FLAG=9:71,3:94,9:82
  lauf elza_s${s}_mit  $s $ELZA
  lauf elza_s${s}_ohne $s $ELZA RE15_SET_FLAG=9:83
done
for s in $SKALEN; do
  x0=$((131*s)); y0=$((75*s)); x1=$((145*s+s-1)); y1=$((87*s+s-1))
  for w in leon elza; do
    r=$("$PROBE" "$OUT/${w}_s${s}_mit/f.ppm" "$OUT/${w}_s${s}_ohne/f.ppm" $x0 $y0 $x1 $y1)
    pl=$(grep -o "Spieler-Familie PL0[0-9]" "$OUT/${w}_s${s}_mit/debug.log" | head -1)
    sl=$(grep -o "\[schluessel\] ROOM10F[01]: [A-Z ]*KEY" "$OUT/${w}_s${s}_mit/debug.log" | head -1)
    echo "$w s$s | $r | $pl | $sl"
  done
done | tee "$OUT/ergebnis.txt"
