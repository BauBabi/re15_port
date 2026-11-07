#!/usr/bin/env bash
# lauf4.sh - Abnahme 4 Spur AN: eigener Lauf im ECHTEN Fenster mit der eigenen exe-Kopie (w2abn/re15_pc_w2abn.exe, sha256
# 5e058b2f..., = Arbeitsbaum r41_annette Kopf cefd57402), Renderer des Nutzers, kein AUTOSHOT, kein SOFTWARE_RENDER.
# Bilder per RE15_FRAMEDUMP, Ende per RE15_EXIT_AT oder timeout auf die EIGENE PID (exe-Name je Lauf eindeutig).
#   lauf4.sh <name> <zeitlimit-s> <eingabe-datei | -> [ENV=WERT ...]
# Ausgabe: w2abn/an4/laeufe/<name>/ (debug.log, stderr.log, state.log, f<NNNNNN>.ppm, env.txt)
set -uo pipefail
W2=/c/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/w2abn
NAME="${1:?name}"; LIMIT="${2:?zeitlimit}"; EING="${3:?eingabe}"; shift 3
LAUF="$W2/an4/laeufe/$NAME"
rm -rf "$LAUF"; mkdir -p "$LAUF"
EXEN="re15_pc_an4_${NAME}_$$"
cp "$W2/re15_pc_w2abn.exe" "$W2/an4/$EXEN.exe" || exit 2
LAUFW=$(cygpath -m "$LAUF")
export RE15_NO_INTRO=1 RE15_WINDOW_SCALE=2 SDL_ASSERT=always_ignore
export RE15_TITLE_SHOT="$LAUFW/titel.bmp" RE15_TITLE_SHOT_AF=2
export RE15_STATE_LOG="$LAUFW/state.log" RE15_SPAWN_DIAG=1
# Kein Audiogeraet in der Sitzung (WASAPI: kein Endpunkt): Ton wie Abnahme 3 synchron mitschneiden, dann ist die Audio-Schicht aktiv.
export RE15_AUDIO_CAP_SYNC="$LAUFW/ton.raw"
if [ "$EING" != "-" ]; then export RE15_INPUT_SCRIPT="$(tr -d ' \r\n' < "$EING")"; fi
for kv in "$@"; do export "${kv//@LAUF@/$LAUFW}"; done
cd "$LAUF" || exit 2
env | grep '^RE15_' | sort > env.txt
T0=$(date +%s)
timeout "$LIMIT" "$W2/an4/$EXEN.exe" >stdout.log 2>stderr.log
RC=$?
echo "$NAME exit=$RC Dauer=$(( $(date +%s) - T0 )) s Bilder: $(ls "$LAUF"/f*.ppm 2>/dev/null | wc -l)" | tee -a "$LAUF/ergebnis.txt"
rm -f "$W2/an4/$EXEN.exe"
