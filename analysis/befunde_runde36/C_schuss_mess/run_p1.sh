#!/bin/bash
# Runde 36 Spur C — Messung Punkt 1 an der ECHTEN exe (ROOM1140, RE2-KI = Vorgabe).
# Aufruf: bash analysis/befunde_runde36/C_schuss_mess/run_p1.sh <tag> [waffe]   -> mess_r36c/<tag>/{tisch,fresser}/{debug,state,wf}.log
# Lauf "tisch":   Leon (200,-10300) Blick -z (rot 1024): Konferenztisch (SCA-Zelle 6, z -17450..-11350)
#                 zwischen Leon und den Zombies (z -19600/-21600). Original: Sichtlinie FUN_8001b9b4
#                 blockiert -> KEIN Treffer.
# Lauf "fresser": Leon am Tuerpunkt (-7600,-17600) Blick +x (rot 0): alle fuenf Fresser (1D3=80) in
#                 6122..8748 Entfernung (> 0xFA0 = 4000, kein Wecken). RE2: Gate 2/4 -> KEIN Treffer.
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r36_schuss"
TAG="${1:-vorher}"
WAFFE="${2:-20}"
EXE_DIR="$BAUM/re15_port/build/platform/pc"
EXE="$EXE_DIR/re15_r36c.exe"
cp -f "$EXE_DIR/re15_pc.exe" "$EXE"
OUT="$BAUM/mess_r36c/$TAG"
mkdir -p "$OUT"

lauf() {   # name pos
    local name="$1" pos="$2"
    local wd="$OUT/$name"
    rm -rf "$wd"; mkdir -p "$wd"
    ( cd "$wd" && \
      RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
      RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=1140@250 RE15_PLAYER_POS="$pos" \
      RE15_AI_FLAVOR=re2 RE15_GIVE="$WAFFE:6" RE15_EQUIP="$WAFFE" \
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT="M0.6,MA1.5,M1.0,W4" \
      RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log RE15_EXIT_AT="200#1140" \
      timeout 240 "$EXE" > stdout.txt 2> stderr.txt; echo "exit=$?" > exit.txt )
    echo "== $name ($pos) $(cat "$wd/exit.txt")"
    # HP je Platz: erste und letzte Zeile nach dem Raumwechsel
    grep -o "\[[0-9]* t=[0-9a-f]* st=[0-9]* ss1=[0-9]* [^]]*\] hp=-\?[0-9]*" "$wd/state.log" | \
      awk '{ split($1,a,"["); slot=a[2]; hp=$NF; sub("hp=","",hp); if (!(slot in first)) first[slot]=hp; last[slot]=hp; st[slot]=$3" "$4 }
           END { for (s in first) printf "   Platz %s: hp %s -> %s  (%s)\n", s, first[s], last[s], st[s] }' | sort
    grep -c "SCHUSS\|FIRE\|fire" "$wd/wf.log" 2>/dev/null | sed 's/^/   wf.log Schuss-Zeilen: /'
}

lauf tisch   "200,-10300,1024"
lauf fresser "-7600,-17600,0"
