#!/usr/bin/env bash
# Abnahme-2-Lauf (unabhaengig): eigene exe-Kopie aus w2abn/exe, eigenes Laufverzeichnis, Renderer des Nutzers
# (kein RE15_SOFTWARE_RENDER, kein RE15_AUTOSHOT), Bildbelege per RE15_FRAMEDUMP, Ende per RE15_EXIT_AT, Notbremse timeout
# (nur der eigene Kindprozess). RE15_RE2_ASSET_ROOT = private Kopie der HEAD-Blobs BAHNSTEIG_FORM.EMD/.TIM.
#   lauf.sh <name> <limit-s> <envdatei>
set -uo pipefail
W=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/w2abn
NAME="$1"; LIMIT="$2"; ENVF="$3"
LAUF="$W/laeufe/$NAME"
rm -rf "$LAUF"; mkdir -p "$LAUF/fd"
EXEN="re15_pc_w2abn_${NAME}_$$"
cp -p "$W/exe/re15_pc_w2abn.exe" "$W/exe/$EXEN.exe" || exit 2
export PATH="/c/msys64/mingw64/bin:$PATH"
export SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_WINDOW_SCALE=2
export RE15_TITLE_SHOT=titel.bmp RE15_TITLE_SHOT_AF=2
export RE15_STATE_LOG=state.log RE15_SPAWN_DIAG=1 RE15_BIRKIN_DBG=1 RE15_SE_DEBUG=1
export RE15_RE2SE_LOG="$(cygpath -m "$LAUF")/re2se.log"
export RE15_RE2_ASSET_ROOT="$(cygpath -m "$W/re2priv")"
while IFS= read -r kv; do kv="${kv%$'\r'}"; [ -n "$kv" ] && export "$kv"; done < "$ENVF"
if [ -n "${FD_SERIE:-}" ]; then export RE15_FRAMEDUMP="${FD_SERIE}:$(cygpath -m "$LAUF/fd")/f"; fi
cd "$LAUF" || exit 2
env | grep -E '^(RE15_|SDL_)' | sort > env.txt
sha256sum "$W/exe/$EXEN.exe" > exe_sha.txt
T0=$(date +%s)
timeout "$LIMIT" "$W/exe/$EXEN.exe" >stdout.log 2>stderr.log
RC=$?
T1=$(date +%s)
echo "exit=$RC dauer=$(( T1 - T0 ))s" | tee lauf_ende.txt
rm -f "$W/exe/$EXEN.exe"
grep -a "Bahnsteig-Form\|EXIT_AT" stderr.log debug.log 2>/dev/null | head -4
echo "fd: $(ls fd | wc -l) Bilder; Lauf: $LAUF"
