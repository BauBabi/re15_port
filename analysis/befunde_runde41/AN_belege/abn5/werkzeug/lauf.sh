#!/usr/bin/env bash
# lauf.sh - Abnahme 5 Spur AN (zweiter Abnehmer): eigener Lauf im ECHTEN Fenster (Renderer des Nutzers, kein AUTOSHOT,
# kein SOFTWARE_RENDER) mit der eigenen Messkopie mein/exe/re15_pc_anabn5.exe (= Arbeitsbaum r41_annette nach 99bbe97ba).
# Je Lauf eine eindeutige Kopie re15_pc_anabn5_<name>_<pid>.exe; Ende per RE15_EXIT_AT oder timeout auf die EIGENE PID.
#   lauf.sh <name> <zeitlimit-s> <eingabe-datei | /dev/stdin | -> [ENV=WERT ...]
# Umgebung 1:1 wie Abnahme 4 lauf4.sh (RE15_AUDIO_CAP_SYNC immer an).
set -uo pipefail
B=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/annb6/abn5/mein
NAME="${1:?name}"; LIMIT="${2:?zeitlimit}"; EING="${3:?eingabe}"; shift 3
LAUF="$B/laeufe/$NAME"
rm -rf "$LAUF"; mkdir -p "$LAUF"
EXEN="re15_pc_anabn5_${NAME}_$$"
cp "$B/exe/re15_pc_anabn5.exe" "$B/exe/$EXEN.exe" || exit 2
sha256sum "$B/exe/$EXEN.exe" | cut -c1-16 > "$LAUF/exe_sha.txt"
LAUFW=$(cygpath -m "$LAUF")
export PATH=/c/msys64/mingw64/bin:$PATH
export RE15_NO_INTRO=1 RE15_WINDOW_SCALE=2 SDL_ASSERT=always_ignore
export RE15_TITLE_SHOT="$LAUFW/titel.bmp" RE15_TITLE_SHOT_AF=2
export RE15_STATE_LOG="$LAUFW/state.log" RE15_SPAWN_DIAG=1
export RE15_AUDIO_CAP_SYNC="$LAUFW/ton.raw"
if [ "$EING" != "-" ]; then export RE15_INPUT_SCRIPT="$(tr -d ' \r\n' < "$EING")"; fi
for kv in "$@"; do export "${kv//@LAUF@/$LAUFW}"; done
cd "$LAUF" || exit 2
env | grep '^RE15_' | sort > env.txt
T0=$(date +%s)
timeout "$LIMIT" "$B/exe/$EXEN.exe" >stdout.log 2>stderr.log
RC=$?
echo "$NAME exit=$RC Dauer=$(( $(date +%s) - T0 )) s Bilder: $(ls "$LAUF"/f*.ppm 2>/dev/null | wc -l) exe=$(cat "$LAUF/exe_sha.txt") $(date '+%H:%M:%S')" | tee -a "$LAUF/ergebnis.txt"
rm -f "$B/exe/$EXEN.exe"
