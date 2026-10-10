#!/bin/bash
# Runde 36 Spur C — Messung Punkt 3 (Kraehe gegen Super Redhawk) an der ECHTEN exe mit Bildern.
# Aufruf: bash analysis/befunde_runde36/C_schuss_mess/run_p3.sh <tag> [exe]
# Aufstellung wie r34 mess_geg.md 3.2: ROOM10C0, Kraehe Slot 3 sitzt bei (5100,3600), Leon 3522,5556,581 (Kollision
# schiebt ihn auf (3918,5556), ~2300 vor der Kraehe), RE15_FORCE_CUT=1 (Cut zeigt beide). RE2-KI, Redhawk (7).
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r36_schuss"
TAG="${1:-nachher}"
EXE_DIR="$BAUM/re15_port/build/platform/pc"
EXE="${2:-$EXE_DIR/re15_r36c.exe}"
[ -f "$EXE" ] || cp -f "$EXE_DIR/re15_pc.exe" "$EXE"
OUT="$BAUM/mess_r36c/p3_$TAG"
rm -rf "$OUT"; mkdir -p "$OUT/bilder"
( cd "$OUT" && \
  RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
  RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=10C0@250 RE15_PLAYER_POS="3522,5556,581" RE15_FORCE_CUT=1 \
  RE15_AI_FLAVOR=re2 RE15_GIVE="7:6" RE15_EQUIP="7" \
  RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT="MD0.6,MDA0.4,MD1.0,W3" \
  RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log RE15_FX_LOG=fx.log \
  RE15_FRAMEDUMP="20-120/2:bilder/f" RE15_EXIT_AT="150#10C0" \
  timeout 300 "$EXE" > stdout.txt 2> stderr.txt; echo "exit=$?" > exit.txt )
echo "== $TAG $(cat "$OUT/exit.txt")"
awk '/^F[0-9]+ /{ n=split($0,p,"["); for(i=2;i<=n;i++){ split(p[i],q," "); slot=q[1]; m=match(p[i],/hp=-?[0-9]+/); hp=substr(p[i],RSTART+3,RLENGTH-3); if((slot in last)&&last[slot]!=hp) print "   " $1, "Platz", slot, last[slot], "->", hp, "|", p[i]; last[slot]=hp } }' "$OUT/state.log" | head -6
ls "$OUT/bilder" | wc -l
