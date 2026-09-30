#!/usr/bin/env bash
# Spur B (Runde 34 Nacht, BAU) — Messlauf am LADE-Weg (Auflage 2 der Gegenpruefung): Spielstand in
# ROOM1150/1151 mit probe_r30_granate_karte schreiben (Spieler (-22250,0,-18500) rot 0 = Westseite des
# Tischs, Blick auf das Modell), CONTINUE, dann Eingabeskript auf der Spielbild-Zeitachse.
# Echte exe unter eigenem Namen (local_build.sh beendet beim Bauen die exe DIESES Bauverzeichnisses),
# beschleunigter Renderer, RE15_FRAMEDUMP, kein AUTOSHOT/SOFTWARE_RENDER.
#   $1 Marke (Unterordner von build/r34n_b/)
#   Umgebung: RAUM (1150|1151), KARTE ("sicherung genommen ..."), SKRIPT, START (Default 60),
#             SERIE ("a-b/s"), EXIT_AT, SEK, SKALA (3), EXTRA_ENV, AUDIO=1
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r34n_hebetisch
SRC_EXE="$WT/re15_port/build/platform/pc/re15_pc.exe"
EXE="$WT/re15_port/build/platform/pc/re15_r34nb.exe"
KARTE_EXE="$WT/re15_port/build/tests/unit/probe_r30_granate_karte.exe"
[ "$SRC_EXE" -nt "$EXE" ] && cp -f "$SRC_EXE" "$EXE"
RAUM="${RAUM:-1150}"
Z="$WT/build/r34n_b/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log befund.log f_*.ppm fb_*.ppm modal.log hebetisch.log cursor.log lauf_rc.txt re15_card.mcr
"$KARTE_EXE" re15_card.mcr "$RAUM" ${KARTE:-} || { echo "Kartenwerkzeug fehlgeschlagen"; exit 3; }
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
[ "${AUDIO:-0}" = "1" ] || export RE15_NOAUDIO=1
export RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
export RE15_WINDOW_SCALE="${SKALA:-3}"
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START="${START:-60}"
export RE15_INPUT_SCRIPT="${SKRIPT:-W60}"
[ -n "${SERIE:-}" ] && export RE15_FRAMEDUMP="${SERIE}:f_"
export RE15_MODAL_LOG="modal.log" RE15_HEBETISCH_LOG="hebetisch.log" RE15_HEBETISCH_CURSOR_LOG="cursor.log"
export RE15_EXIT_AT="${EXIT_AT:-400#$RAUM}"
if [ -n "${EXTRA_ENV:-}" ]; then for kv in $EXTRA_ENV; do export "$kv"; done; fi
timeout -k 5 "${SEK:-150}" "$EXE"
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l)" > lauf_rc.txt
cat lauf_rc.txt
