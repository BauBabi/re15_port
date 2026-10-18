#!/bin/bash
# Messung Spur M am ECHTEN Lade-Weg (Spielstand + CONTINUE, wie integration_r38_itemorte k_lauf): Pinnwand 10F0 (Leon) /
# 10F1 (Elza), Cut 5, Bild 100, Skala 3 (und 4), mit/ohne Taken-Bit.
# Aufruf: mess_continue.sh <exe-kopie> <ausgabeordner> [skalen...]
EXE="$1"; OUT="$2"; shift 2
SKALEN="${*:-3 4}"
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r39_schluessel2"
PROBE="$BAUM/re15_port/build/tests/unit/probe_r38_itemorte_kontrast.exe"
KARTE="$BAUM/re15_port/build/tests/unit/probe_r37_feuerloescher_karte.exe"
mkdir -p "$OUT"
for v in $(env | grep -o '^RE15_[A-Za-z0-9_]*'); do unset "$v"; done
lauf() {   # name skala kartenargs...
  local name="$1" skala="$2"; shift 2
  local d="$OUT/$name"; rm -rf "$d"; mkdir -p "$d"
  ( cd "$d" && "$KARTE" re15_card.mcr "$@" > karte.txt 2>&1 && \
    env RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=$skala \
      RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 \
      RE15_INPUT_SCRIPT=W3 RE15_MSG_LOG=1 RE15_EXIT_AT=120 RE15_FRAMEDUMP=100:fd_100.ppm \
      timeout 300 "$EXE" > stdout.txt 2>&1; echo "exit=$?" > rc.txt )
  [ -f "$d/fd_100.ppm" ] || echo "FEHLT fd_100.ppm: $name ($(cat $d/rc.txt 2>/dev/null))"
}
for s in $SKALEN; do
  lauf leon_s${s}_mit  $s 10F0 0 4000 1024 cut:5 flag:9:71 flag:3:94
  lauf leon_s${s}_ohne $s 10F0 0 4000 1024 cut:5 flag:9:71 flag:3:94 flag:9:82
  lauf elza_s${s}_mit  $s 10F1 0 4000 1024 cut:5 elza
  lauf elza_s${s}_ohne $s 10F1 0 4000 1024 cut:5 elza flag:9:83
done
for s in $SKALEN; do
  x0=$((131*s)); y0=$((75*s)); x1=$((145*s+s-1)); y1=$((87*s+s-1))
  for w in leon elza; do
    r=$("$PROBE" "$OUT/${w}_s${s}_mit/fd_100.ppm" "$OUT/${w}_s${s}_ohne/fd_100.ppm" $x0 $y0 $x1 $y1)
    cr=$(grep -o "CONTINUE: resumed in room [0-9a-f]*" "$OUT/${w}_s${s}_mit/debug.log" | head -1)
    pl=$(grep -o "Spieler-Familie PL0[0-9]" "$OUT/${w}_s${s}_mit/debug.log" | head -1)
    echo "$w s$s CONTINUE | $r | $cr | $pl"
  done
done | tee "$OUT/ergebnis.txt"
