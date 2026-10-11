#!/usr/bin/env bash
# Nachbesserung 1 (Abnahme 0 M1): Debug-Menue-Sprung "108 ELEVATOR" aus ROOM1100 ohne Bank-3-Bits
# 54/55/56, Karte in der Kabine, Abzug je Blatt (1F / 2F / 3F).
# Aufruf: debuglauf.sh <etikett> [<ziel=1080>] [<startraum> <x> <z> <rot>]
set -u
BAUM="C:/workspace/git/reAi_v2/.claude/worktrees/r36_karte"
BUILD="${RE15_BUILD:-$BAUM/re15_port/build}"
EXE="$BUILD/platform/pc/re15_pc.exe"
PROBE="$BUILD/tests/unit/probe_r36_karte.exe"
SP="C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/nb1"
ETI="$1"; ZIEL="${2:-1080}"
START="${3:-1100}"; SX="${4:--26130}"; SZ="${5:--11036}"; SR="${6:-2048}"
ID=$(printf '%04x%04x' $RANDOM $RANDOM)
KOPIE="$(dirname "$EXE")/re15_pc_r36m_nb1_${ID}.exe"
cp "$EXE" "$KOPIE"
for BLATT in 0 1 2; do
    W="$SP/laeufe/${ETI}_b${BLATT}"
    rm -rf "$W"; mkdir -p "$W"; cd "$W"
    "$PROBE" karte re15_card.mcr "$START" "$SX" 0 "$SZ" "$SR" "besucht:$START:$SX:$SZ" > probe.txt 2>&1
    PAD=""
    if [ "$BLATT" = 1 ]; then PAD="100:U"; fi
    if [ "$BLATT" = 2 ]; then PAD="100:U,108:U"; fi
    env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_SOFTWARE_RENDER=1 RE15_WINDOW_SCALE=1 \
        RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 \
        RE15_DEBUG_JUMP="${ZIEL}@gp" ${PAD:+RE15_PAD_AT=$PAD} \
        RE15_INV_OPEN_AT="40#${ZIEL}" RE15_INV_FB_SHOT=karte.bmp RE15_INV_FB_SHOT_AT=85 \
        RE15_EXIT_AT="170#${ZIEL}" \
        timeout 240 "$KOPIE" > exe_out.txt 2>&1
    echo "lauf ${ETI}_b${BLATT} exit=$? bmp=$( [ -f karte.bmp ] && echo ja || echo NEIN )"
    grep -a "JUMP ->\|AUTO-JUMP" debug.log 2>/dev/null | head -3
done
rm -f "$KOPIE"
