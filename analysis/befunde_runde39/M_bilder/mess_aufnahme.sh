#!/bin/bash
# Aufnahme am ECHTEN Lade-Weg (Spielstand vor der Pinnwand (1300,-2900) Blick -z, CONTINUE, Aktion -> Modal -> Ja), wie
# integration_r38_itemorte K1/K2; dazu Skala-2-Bilder (960 -> 640x480) mit/ohne fuer die Sichtbarkeit.
# Aufruf: mess_aufnahme.sh <exe-kopie> <ausgabeordner>
EXE="$1"; OUT="$2"
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r39_schluessel2"
PROBE="$BAUM/re15_port/build/tests/unit/probe_r38_itemorte_kontrast.exe"
KARTE="$BAUM/re15_port/build/tests/unit/probe_r37_feuerloescher_karte.exe"
mkdir -p "$OUT"
for v in $(env | grep -o '^RE15_[A-Za-z0-9_]*'); do unset "$v"; done
lauf() {   # name skala skript ende extra kartenargs...
  local name="$1" skala="$2" skript="$3" ende="$4" extra="$5"; shift 5
  local d="$OUT/$name"; rm -rf "$d"; mkdir -p "$d"
  ( cd "$d" && "$KARTE" re15_card.mcr "$@" > karte.txt 2>&1 && \
    env RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=$skala \
      RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 \
      "RE15_INPUT_SCRIPT=$skript" RE15_MSG_LOG=1 "RE15_EXIT_AT=$ende" $extra \
      timeout 400 "$EXE" > stdout.txt 2>&1; echo "exit=$?" > rc.txt )
}
lauf auf_leon 1 "W1,A0.1,W3.5,A0.1,W4,A0.1,W6" 520 "" 10F0 1300 -2900 1024 cut:5 flag:9:71 flag:3:94
lauf auf_elza 1 "W1,A0.1,W3.5,A0.1,W4,A0.1,W6" 520 "" 10F1 1300 -2900 1024 cut:5 elza
lauf leon_s2_mit  2 W3 120 RE15_FRAMEDUMP=100:f.ppm 10F0 0 4000 1024 cut:5 flag:9:71 flag:3:94
lauf leon_s2_ohne 2 W3 120 RE15_FRAMEDUMP=100:f.ppm 10F0 0 4000 1024 cut:5 flag:9:71 flag:3:94 flag:9:82
lauf elza_s2_mit  2 W3 120 RE15_FRAMEDUMP=100:f.ppm 10F1 0 4000 1024 cut:5 elza
lauf elza_s2_ohne 2 W3 120 RE15_FRAMEDUMP=100:f.ppm 10F1 0 4000 1024 cut:5 elza flag:9:83
{
for w in leon elza; do
  echo "auf_$w: $(grep -o 'CONTINUE: resumed in room [0-9a-f]*' $OUT/auf_$w/debug.log | head -1) | $(grep -o 'Spieler-Familie PL0[0-9]' $OUT/auf_$w/debug.log | head -1)"
  grep -h "genommen\|Flag (4,0xEC)\|msgtext\] room=10f[01] id=4" "$OUT/auf_$w/debug.log" | head -4
done
for w in leon elza; do
  echo "$w s2 CONTINUE | $("$PROBE" "$OUT/${w}_s2_mit/f.ppm" "$OUT/${w}_s2_ohne/f.ppm" 262 150 291 175)"
done
} | tee "$OUT/ergebnis.txt"
