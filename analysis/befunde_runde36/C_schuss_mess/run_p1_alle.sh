#!/bin/bash
# Runde 36 Spur C — Punkt 1 je WAFFE an der ECHTEN exe (ROOM1140, RE2-KI = Vorgabe), Wortlaut-Pruefung
# "ALLE Schusswaffen": Laeufe tisch/fresser wie run_p1.sh fuer jede Schusswaffen-Id (Item-Katalog
# inventory_common.c s_item_names): 3 Browning HP, 4 SIG P228, 5 Beretta M93R, 6 Glock 18, 7 Super Redhawk,
# 8 Remington M870, 12 Ingram M10, 13 SPAS-12, 14 Flammenwerfer, 15/16/17 Granatwerfer (Explosiv/Saeure/Brand),
# 18 Raketenwerfer, 19 H&K MC51, 20 Colt Python.
# Aufruf: bash analysis/befunde_runde36/C_schuss_mess/run_p1_alle.sh <tag> [ids...]  -> mess_r36c/<tag>/w<id>/{tisch,fresser}/
# Zusammenfassung: mess_r36c/<tag>/zusammenfassung.txt (HP je Platz vorher -> nachher je Lauf).
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r36_schuss"
TAG="${1:-alle}"; shift || true
IDS="${*:-3 4 5 6 7 8 12 13 14 15 16 17 18 19 20}"
EXE_DIR="$BAUM/re15_port/build/platform/pc"
EXE="$EXE_DIR/re15_r36c.exe"
cp -f "$EXE_DIR/re15_pc.exe" "$EXE"
OUT="$BAUM/mess_r36c/$TAG"
mkdir -p "$OUT"
SUM="$OUT/zusammenfassung.txt"
: > "$SUM"

lauf() {   # waffe name pos
    local waffe="$1" name="$2" pos="$3"
    local wd="$OUT/w$waffe/$name"
    rm -rf "$wd"; mkdir -p "$wd"
    ( cd "$wd" && \
      RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
      RE15_WINDOW_SCALE=1 RE15_DEBUG_JUMP=1140@250 RE15_PLAYER_POS="$pos" \
      RE15_AI_FLAVOR=re2 RE15_GIVE="$waffe:6" RE15_EQUIP="$waffe" \
      RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT="M0.6,MA1.5,M1.0,W4" \
      RE15_STATE_LOG=state.log RE15_WAFFEN_LOG=wf.log RE15_FX_LOG=fx.log RE15_EXIT_AT="200#1140" \
      timeout 240 "$EXE" > stdout.txt 2> stderr.txt; echo "exit=$?" > exit.txt )
    local hp
    hp=$(grep -o "\[[0-9]* t=[0-9a-f]* st=[0-9]* ss1=[0-9]* [^]]*\] hp=-\?[0-9]*" "$wd/state.log" | \
      awk '{ split($1,a,"["); slot=a[2]; hp=$NF; sub("hp=","",hp); if (!(slot in first)) first[slot]=hp; last[slot]=hp; st[slot]=$3" "$4 }
           END { for (s in first) printf "P%s %s->%s(%s) ", s, first[s], last[s], st[s] }' )
    local schuss
    schuss=$(grep -c "arms_rec=0\|re2arms\|ARMS" "$wd/wf.log" 2>/dev/null)
    printf "w%-2s %-7s %s | wf-Zeilen %s | %s\n" "$waffe" "$name" "$(cat "$wd/exit.txt")" "$schuss" "$hp" | tee -a "$SUM"
}

for w in $IDS; do
    lauf "$w" tisch   "200,-10300,1024"
    lauf "$w" fresser "-7600,-17600,0"
done
echo "FERTIG" >> "$SUM"
