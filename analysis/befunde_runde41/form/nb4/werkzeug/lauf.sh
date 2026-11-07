#!/usr/bin/env bash
# NB4-Lauf: eigene exe-Kopie (neben der Bau-exe, Asset-Wurzel = exe-Verzeichnis), eigenes Laufverzeichnis, Renderer des Nutzers
# (kein RE15_SOFTWARE_RENDER/AUTOSHOT), RE15_FRAMEDUMP-Serie, Ende per RE15_EXIT_AT, Notbremse timeout nur auf den eigenen Kindprozess.
#   lauf.sh <name> <exe> <char 0|1> <weg w|v> <exit_bild> <fd_serie>
set -uo pipefail
W=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formnb4
NAME="$1"; EXE="$2"; CH="$3"; WEG="$4"; EXITB="$5"; FD="$6"
LAUF="$W/laeufe/$NAME"
rm -rf "$LAUF"; mkdir -p "$LAUF/fd"
EXED=/c/workspace/git/reAi_v2/.claude/worktrees/r41_form_nb4/re15_port/build/platform/pc
EXEN="$EXED/re15_pc_nb4_${NAME}_$$.exe"
cp -p "$EXE" "$EXEN" || exit 2
S=""
for i in 1 2 3 4 5 6 7; do S="${S}A0.1000,W0.1000,"; done
S="${S}U15.0000,"
for i in 1 2 3 4 5 6 7 8 9 10; do S="${S}A0.1000,W0.1000,"; done
S="${S}L0.0333,U2.1667,R0.0333,U10.7000,"
for i in 1 2 3 4 5 6 7 8 9; do S="${S}A0.1000,W0.1000,"; done
S="${S}A0.1000,W0.6000,U4.9000,W2"
[ "$WEG" = v ] && S="${S},D2.0"
for i in $(seq 1 24); do S="${S},MA4"; done
R=5090; [ "$CH" = 1 ] && R=5091
export PATH="/c/msys64/mingw64/bin:$PATH"
export SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_WINDOW_SCALE=2 SDL_AUDIODRIVER=dummy
export RE15_TITLE_SHOT=titel.bmp RE15_TITLE_SHOT_AF=2
[ "$CH" = 1 ] && export RE15_TITLE_SHOT_CHAR=1
export RE15_STATE_LOG=state.log RE15_SPAWN_DIAG=1 RE15_BIRKIN_DBG=1
export RE15_DEBUG_JUMP=${R}@250 RE15_PLAYER_POS=27200,4900,2048,0 RE15_SET_FLAG=3:89
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=6 RE15_INPUT_SCRIPT="$S"
export RE15_GIVE=3:400,21:250 RE15_EQUIP=3
if [ "$WEG" = a ]; then   # Abnahme-2-Lauf a5090/a5091: Debug-Sprung Wagen B, Angriffe ohne Feuer, Griff, Wache (M-B)
  unset RE15_GIVE RE15_EQUIP RE15_SET_FLAG
  export RE15_PLAYER_POS=500,-9175,2048,0 RE15_INPUT_SCRIPT=A0.1,W1,U8.5,W60 RE15_INPUT_SCRIPT_START=100
fi
export RE15_EXIT_AT="${EXITB}#${R}"
export RE15_BF_KAMLOG="$(cygpath -m "$LAUF")/kam.log" RE15_SLOTDECKUNG="$(cygpath -m "$LAUF")/deck.log"
[ -n "$FD" ] && export RE15_FRAMEDUMP="${FD}:$(cygpath -m "$LAUF/fd")/f"
cd "$LAUF" || exit 2
env | grep -E '^(RE15_|SDL_)' | sort > env.txt
sha256sum "$EXEN" > exe_sha.txt
T0=$(date +%s)
timeout 600 "$EXEN" >stdout.log 2>stderr.log
RC=$?
T1=$(date +%s)
echo "exit=$RC dauer=$(( T1 - T0 ))s" | tee lauf_ende.txt
rm -f "$EXEN"
echo "fd: $(ls fd | wc -l) Bilder; Lauf: $LAUF"
