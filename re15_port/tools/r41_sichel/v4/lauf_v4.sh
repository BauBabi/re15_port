#!/usr/bin/env bash
# Spur SF "V4" (Kopie von v3/lauf_v3.sh): Szenenlauf Hangar 50A0 mit RE15_FRAMEDUMP (wie nb2neu/lauf6.sh) und einer VARIANTE des Modells,
# ohne das Spiel-Asset zu aendern: RE15_CD_ROOT zeigt auf eine Overlay-Wurzel, die nur EMD/EM30_HANGAR.EMD enthaelt; alle
# anderen Dateien findet der Port in der naechsten CD-Wurzel (asset_root_pc.c rr_read_list, Wurzel fuer Wurzel).
# lauf_v2.sh <laufordner> <emd-datei|-> [exitbild] [env...]   (EMD "-" = Spiel-Asset, kein Overlay)
# EXE_SRC = zu startende exe (Standard: Bau dieses Arbeitsbaums). Eigene exe-Kopie, nur eigene PID.
set -u
BAUM="${BAUM:-C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel}"   # V8: per Umgebung auf den eigenen Arbeitsbaum setzbar
EXE_SRC="${EXE_SRC:-$BAUM/re15_port/build/platform/pc/re15_pc.exe}"
NAME="re15_sfv4_$$"
D="$1"; EMD="$2"; EX="${3:-420}"; shift 3 2>/dev/null || shift $#
mkdir -p "$D/fd"; rm -f "$D"/fd/*.ppm "$D"/state.log "$D"/out.txt "$D"/debug.log "$D"/griff.log "$D"/bein.log
CDR="$BAUM/re15_port/shared_assets/PSX"
if [ "$EMD" != "-" ]; then
  mkdir -p "$D/cd/EMD"; cp -f "$EMD" "$D/cd/EMD/EM30_HANGAR.EMD" || exit 2
  CDR="$D/cd"
fi
cp -f "$EXE_SRC" "$D/$NAME.exe" || exit 2
cd "$D" || exit 2
export PATH="/c/msys64/mingw64/bin:$PATH"
env SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
    RE15_CD_ROOT="$CDR" RE15_HANGAR_LOG=1 RE15_STATE_LOG=state.log RE15_GRIFF_LOG=griff.log RE15_BEIN_LOG=bein.log \
    RE15_SET_FLAG=8:42,8:43 RE15_DEBUG_JUMP=50A0@20 RE15_PLAYER_POS=-12406,4381,1024 \
    RE15_INPUT_SCRIPT="${INP:-W0.5,UX3.6,W0.3,L0.27,W0.3,UX2.5,W12,UX1.5,W10}" RE15_EXIT_AT="$EX" \
    RE15_FRAMEDUMP="${FD:-225-415/1}:fd/f" "$@" \
    timeout 400 "./$NAME.exe" > out.txt 2>&1
echo "EXIT=$?"
rm -f "$NAME.exe"
ls fd/*.ppm 2>/dev/null | wc -l
grep -a "\[asset\] cd-root\|EM30: Ersatzmodell\|EM30 loaded" debug.log out.txt 2>/dev/null | head -4
