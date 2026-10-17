#!/usr/bin/env bash
# Runde 38 Spur A — Messlauf Feuerloescher (Port-Modell gen/feuerloescher_r37.inc) unter dem Rueckseiten-Verwurf:
# Spielstand per probe_r37_feuerloescher_karte (Lauf E/F aus tests/integration/test_r37_feuerloescher.cmake), CONTINUE,
# Framedump. Zweck: Nebenwirkung des Verwurfs auf ein PORT-GEBAUTES Modell (Wicklung, A_belege/wicklung_props.py).
#   $1 Marke (Unterordner von re15_port/build/r38_a/)
#   ART  e = ROOM1050 Wand (Lauf E, Cut 6)   f = ROOM1090 Boden (Lauf F, Cut 5)
#   EXE_NAME  re15_pc_r38a_alt (1bcbd0ff, ohne Verwurf) | re15_pc_r38a_vorwicklung (Verwurf, Modell alt) |
#             re15_pc_r38a (aktueller Bau)
set -uo pipefail
WT=/c/workspace/git/reAi_v2/.claude/worktrees/r38_cursor
PCD="$WT/re15_port/build/platform/pc"
[ "$PCD/re15_pc.exe" -nt "$PCD/re15_pc_r38a.exe" ] && cp -f "$PCD/re15_pc.exe" "$PCD/re15_pc_r38a.exe"
EXE="$PCD/${EXE_NAME:-re15_pc_r38a}.exe"
KARTE_EXE="$WT/re15_port/build/tests/unit/probe_r37_feuerloescher_karte.exe"
Z="$WT/re15_port/build/r38_a/$1"; mkdir -p "$Z"; cd "$Z" || exit 2
rm -f debug.log befund.log f_*.ppm lauf_rc.txt re15_card.mcr
case "${ART:-e}" in
  e) KARTE="1050;18900;-20800;0;cut:6"; SKRIPT="W3"; BILD=120 ;;
  f) KARTE="1090;0;250;0;y:-1800;cut:5;flag:9:0x86;flag:3:0x85;flag:3:0x81;flag:3:0xbb;flag:3:0x6e"; SKRIPT="W2"; BILD=98 ;;
esac
IFS=";" read -r -a KARGS <<< "$KARTE"
"$KARTE_EXE" re15_card.mcr "${KARGS[@]}" > karte.txt || { echo "Kartenwerkzeug fehlgeschlagen"; exit 3; }
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
export RE15_WINDOW_SCALE="${SKALA:-1}"
export RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 RE15_INPUT_SCRIPT="$SKRIPT"
export RE15_FRAMEDUMP="$BILD:f_${SKALA:-1}.ppm"
export RE15_EXIT_AT="$(( BILD + 2 ))"
if [ -n "${EXTRA_ENV:-}" ]; then for kv in $EXTRA_ENV; do export "$kv"; done; fi
timeout -k 5 "${SEK:-150}" "$EXE"
echo "rc=$? Bilder=$(ls f_*.ppm 2>/dev/null | wc -l)" > lauf_rc.txt
cat lauf_rc.txt
