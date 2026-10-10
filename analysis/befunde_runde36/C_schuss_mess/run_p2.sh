#!/bin/bash
# Runde 36 Spur C — Messung Punkt 2 (Blut-Anker) an der ECHTEN exe mit Bildern.
# Aufruf: bash analysis/befunde_runde36/C_schuss_mess/run_p2.sh <tag> [exe]
# ROOM1140, RE2-KI, Leon (-5000,-19600) Blick +x: Platz 2 (-1800,-19600) in 3200 (< 4000 -> wacht auf),
# Pistole 3; Eingabe: 1,2 s warten (Aufstehen), dann zielen + Abzug halten. Framedump-Serie um den Treffer,
# fx.log liefert das Spielbild des ersten Blut-Spawns (id=0), state.log den HP-Verlust.
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r36_schuss"
TAG="${1:-nachher}"
EXE_DIR="$BAUM/re15_port/build/platform/pc"
EXE="${2:-$EXE_DIR/re15_r36c.exe}"
[ -f "$EXE" ] || cp -f "$EXE_DIR/re15_pc.exe" "$EXE"
OUT="$BAUM/mess_r36c/p2_$TAG"
rm -rf "$OUT"; mkdir -p "$OUT/bilder"
( cd "$OUT" && \
  RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
  RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=1140@250 RE15_PLAYER_POS="-5000,-19600,0" \
  RE15_AI_FLAVOR=re2 RE15_GIVE="3:18" RE15_EQUIP="3" \
  RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT="W1.2,M0.6,MA2.0,M0.5,W2" \
  RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log RE15_FX_LOG=fx.log \
  RE15_FRAMEDUMP="52-62/1:bilder/f" RE15_EXIT_AT="220#1140" \
  timeout 300 "$EXE" > stdout.txt 2> stderr.txt; echo "exit=$?" > exit.txt )
echo "== $TAG $(cat "$OUT/exit.txt")"
grep -m3 "^id=0 " "$OUT/fx.log" | sed 's/ xlat.*F=/ F=/'
awk '/^F[0-9]+ /{ n=split($0,p,"["); for(i=2;i<=n;i++){ split(p[i],q," "); slot=q[1]; m=match(p[i],/hp=-?[0-9]+/); hp=substr(p[i],RSTART+3,RLENGTH-3); if((slot in last)&&last[slot]!=hp&&last[slot]>0) print "   " $1, "Platz", slot, last[slot], "->", hp; last[slot]=hp } }' "$OUT/state.log" | head -5
ls "$OUT/bilder" | wc -l
