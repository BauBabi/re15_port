#!/usr/bin/env bash
# Abnahme 4 Stufe 3 (eigener Lauf des Abnehmers, unabhaengig von Bauer und Vorgaenger-Abnehmern): eigene exe-Kopie
# re15_pc_formabn3_<lauf>_<pid>.exe aus abn4e/exe/re15_pc_formabn3.exe (Kopf 81ec52a59, exe-Stand 0c53ef647, sha256 aa370951...)
# bzw. mit 4. Argument "vorher" aus re15_pc_formabn3_vorher.exe (Stand 2e3713c74, sha256 f4deb1a3...). Eigenes Laufverzeichnis,
# Renderer des Nutzers (kein RE15_SOFTWARE_RENDER, kein RE15_AUTOSHOT), Bildbelege per RE15_FRAMEDUMP-Serie (FD_SERIE aus der
# env-Datei), Ende per RE15_EXIT_AT, Notbremse timeout nur auf den eigenen Kindprozess (nie taskkill /IM).
# RE15_RE2_ASSET_ROOT = private Kopie der HEAD-Blobs BAHNSTEIG_FORM.EMD/.TIM (sha256 16eb6ea8... / ec280802...).
#   lauf.sh <name> <limit-s> <envdatei> [vorher]
set -uo pipefail
A=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formnb4/abn4e
NAME="$1"; LIMIT="$2"; ENVF="$3"; VAR="${4:-}"
LAUF="$A/laeufe/$NAME"
rm -rf "$LAUF"; mkdir -p "$LAUF/fd"
if [ "$VAR" = "vorher" ]; then SRC="$A/exe/re15_pc_formabn3_vorher.exe"; EXEN="re15_pc_formabn3_vorher_${NAME}_$$"; else SRC="$A/exe/re15_pc_formabn3.exe"; EXEN="re15_pc_formabn3_${NAME}_$$"; fi
cp -p "$SRC" "$A/exe/$EXEN.exe" || exit 2
export PATH="/c/msys64/mingw64/bin:$PATH"
export SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_WINDOW_SCALE=2 SDL_AUDIODRIVER=dummy
export RE15_TITLE_SHOT=titel.bmp RE15_TITLE_SHOT_AF=2
export RE15_STATE_LOG=state.log RE15_SPAWN_DIAG=1 RE15_BIRKIN_DBG=1 RE15_SE_DEBUG=1
export RE15_RE2SE_LOG="$(cygpath -m "$LAUF")/re2se.log"
export RE15_RE2_ASSET_ROOT="$(cygpath -m "$A/re2priv")"
export RE15_BF_KAMLOG="$(cygpath -m "$LAUF")/kam.log" RE15_SLOTDECKUNG="$(cygpath -m "$LAUF")/deck.log"
while IFS= read -r kv; do kv="${kv%$'\r'}"; [ -n "$kv" ] && export "$kv"; done < "$ENVF"
if [ -n "${FD_SERIE:-}" ]; then export RE15_FRAMEDUMP="${FD_SERIE}:$(cygpath -m "$LAUF/fd")/f"; fi
cd "$LAUF" || exit 2
env | grep -E '^(RE15_|SDL_|FD_)' | sort > env.txt
sha256sum "$A/exe/$EXEN.exe" > exe_sha.txt
T0=$(date +%s)
timeout "$LIMIT" "$A/exe/$EXEN.exe" >stdout.log 2>stderr.log &
KIND=$!
echo "pid=$KIND exe=$EXEN" > pid.txt
wait $KIND
RC=$?
T1=$(date +%s)
echo "exit=$RC dauer=$(( T1 - T0 ))s" | tee lauf_ende.txt
rm -f "$A/exe/$EXEN.exe"
echo "fd: $(ls fd | wc -l) Bilder; Lauf: $LAUF"
