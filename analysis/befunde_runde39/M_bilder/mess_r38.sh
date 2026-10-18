#!/bin/bash
# Messung Spur M wie integration_r38_itemorte (Spielstand + CONTINUE, Bild 100, Skala 1): B2/B3 = Cut 5 (Fenster
# x131..145 y75..87), V3/V5 = RE15_FORCE_CUT=6 (Fenster x76..103 y62..81), Leon 10F0 / Elza 10F1, mit/ohne Taken-Bit.
# Aufruf: mess_r38.sh <exe-kopie> <ausgabeordner>
EXE="$1"; OUT="$2"
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r39_schluessel2"
PROBE="$BAUM/re15_port/build/tests/unit/probe_r38_itemorte_kontrast.exe"
KARTE="$BAUM/re15_port/build/tests/unit/probe_r37_feuerloescher_karte.exe"
mkdir -p "$OUT"
for v in $(env | grep -o '^RE15_[A-Za-z0-9_]*'); do unset "$v"; done
lauf() {   # name forcecut kartenargs...
  local name="$1" fc="$2"; shift 2
  local d="$OUT/$name"; rm -rf "$d"; mkdir -p "$d"
  local extra=""; [ "$fc" != "-" ] && extra="RE15_FORCE_CUT=$fc"
  ( cd "$d" && "$KARTE" re15_card.mcr "$@" > karte.txt 2>&1 && \
    env RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=1 \
      RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 \
      RE15_INPUT_SCRIPT=W3 RE15_MSG_LOG=1 RE15_EXIT_AT=120 RE15_FRAMEDUMP=100:fd_100.ppm $extra \
      timeout 300 "$EXE" > stdout.txt 2>&1; echo "exit=$?" > rc.txt )
  [ -f "$d/fd_100.ppm" ] || echo "FEHLT fd_100.ppm: $name ($(cat $d/rc.txt 2>/dev/null))"
}
lauf b2_mit  - 10F0 0 4000 1024 cut:5 flag:9:71 flag:3:94
lauf b2_ohne - 10F0 0 4000 1024 cut:5 flag:9:71 flag:3:94 flag:9:82
lauf b3_mit  - 10F1 0 4000 1024 cut:5 elza
lauf b3_ohne - 10F1 0 4000 1024 cut:5 elza flag:9:83
lauf v3_mit  6 10F0 0 4000 1024 cut:5 flag:9:71 flag:3:94
lauf v3_ohne 6 10F0 0 4000 1024 cut:5 flag:9:71 flag:3:94 flag:9:82
lauf v5_mit  6 10F1 0 4000 1024 cut:5 elza
lauf v5_ohne 6 10F1 0 4000 1024 cut:5 elza flag:9:83
{
for b in b2 b3; do echo "$b Cut5 s1 | $("$PROBE" "$OUT/${b}_mit/fd_100.ppm" "$OUT/${b}_ohne/fd_100.ppm" 131 75 145 87)"; done
for b in v3 v5; do echo "$b Cut6 s1 | $("$PROBE" "$OUT/${b}_mit/fd_100.ppm" "$OUT/${b}_ohne/fd_100.ppm" 76 62 103 81)"; done
} | tee "$OUT/ergebnis.txt"
