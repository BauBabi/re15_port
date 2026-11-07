#!/usr/bin/env bash
# lauf6.sh - Nachbesserung 6 Spur AN: eigener Lauf im echten Fenster mit eigener exe-Kopie des Arbeitsbaums r41_annette
# (Renderer des Nutzers, kein AUTOSHOT/SOFTWARE_RENDER). Bilder per RE15_FRAMEDUMP, Ende per RE15_EXIT_AT oder timeout auf die
# eigene PID (exe-Name je Lauf eindeutig, nie taskkill /IM).
#   lauf6.sh <name> <zeitlimit-s> <eingabe> [ENV=WERT ...]
set -uo pipefail
B=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/annb6
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r41_annette
NAME="${1:?name}"; LIMIT="${2:?zeitlimit}"; EING="${3:?eingabe}"; shift 3
LAUF="$B/laeufe/$NAME"
rm -rf "$LAUF"; mkdir -p "$LAUF" "$B/exe"
EXEN="re15_pc_annb6_${NAME}_$$"
cp "$WT/re15_port/build/platform/pc/re15_pc.exe" "$B/exe/$EXEN.exe" || exit 2
sha256sum "$B/exe/$EXEN.exe" | cut -c1-16 > "$LAUF/exe_sha.txt"
LAUFW=$(cygpath -m "$LAUF")
export PATH=/c/msys64/mingw64/bin:$PATH
export RE15_NO_INTRO=1 RE15_WINDOW_SCALE=2 SDL_ASSERT=always_ignore
export RE15_TITLE_SHOT="$LAUFW/titel.bmp" RE15_TITLE_SHOT_AF=2
export RE15_STATE_LOG="$LAUFW/state.log" RE15_SPAWN_DIAG=1
export RE15_AUDIO_CAP_SYNC="$LAUFW/ton.raw"
export RE15_INPUT_SCRIPT="$EING"
for kv in "$@"; do export "$kv"; done
cd "$LAUF" || exit 2
env | grep '^RE15_' | sort > env.txt
T0=$(date +%s)
timeout "$LIMIT" "$B/exe/$EXEN.exe" >stdout.log 2>stderr.log
RC=$?
echo "$NAME exit=$RC Dauer=$(( $(date +%s) - T0 )) s Bilder: $(ls "$LAUF"/f*.ppm 2>/dev/null | wc -l)" | tee -a "$LAUF/ergebnis.txt"
rm -f "$B/exe/$EXEN.exe"
