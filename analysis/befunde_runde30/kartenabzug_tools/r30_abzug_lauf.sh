#!/usr/bin/env bash
# Runde 30 karten-marken-abzug: EIN Kartenabzug aus dem Stand des Nutzers.
#   r30_abzug_lauf.sh <name> <anzahl_hoch> [<slot>]
# Laedt Slot <slot> (Vorgabe 2 = Block 3 der Karte, ROOM1070) ueber den echten Weg
# Titel -> LOAD GAME -> Slot (RE15_CONTINUE_TEST + RE15_CARD_AUTO), oeffnet im Raum
# die Karte (RE15_INV_OPEN_AT), drueckt <anzahl_hoch> mal HOCH (Blatt wechseln) und
# zieht den Software-Bildspeicher des Statusschirms ab (RE15_INV_FB_SHOT).
# Die exe ist die EIGENE aus build_r30_kartenabzug; die Karte liegt als re15_card.mcr
# daneben (Kopie der Nutzer-Karte, wird vor jedem Lauf frisch kopiert).
# Umgebung: VORLAUF (Skript-Anfang, Vorgabe W25), SHOT_AT (Inventar-Bild, Vorgabe 700),
#           LAUFZEIT (Sekunden, Vorgabe 75), EXTRA_ENV ("A=1 B=2"), KARTE (andere .mcr).
set -u
REPO=/c/workspace/git/reAi_v2
EXE_DIR=$REPO/re15_port/build_r30_kartenabzug/platform/pc
OUT=$REPO/build/r30_karten-marken-abzug
NAME=$1; HOCH=$2; SLOT=${3:-2}
KARTE=${KARTE:-$REPO/analysis/befunde_runde30/nutzer_marken/re15_card_nutzer_2026-09-27.mcr}
mkdir -p "$OUT"
cp "$KARTE" "$EXE_DIR/re15_card.mcr"
SKRIPT="${VORLAUF:-W25}"
i=0
while [ $i -lt $HOCH ]; do SKRIPT="$SKRIPT,U0.2,W1"; i=$((i+1)); done
SKRIPT="$SKRIPT,W30"
rm -f "$OUT/$NAME.bmp"
cd "$EXE_DIR"
env RE15_NOAUDIO=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=$SLOT \
    RE15_INV_OPEN_AT="${OPEN_AT:-30#1070}" \
    RE15_INPUT_SCRIPT="$SKRIPT" RE15_INPUT_SCRIPT_START=0 \
    RE15_INV_FB_SHOT="C:/workspace/git/reAi_v2/build/r30_karten-marken-abzug/$NAME.bmp" \
    RE15_INV_FB_SHOT_AT=${SHOT_AT:-700} \
    ${EXTRA_ENV:-} \
    timeout ${LAUFZEIT:-75} ./re15_pc.exe > "$OUT/$NAME.stdout.txt" 2> "$OUT/$NAME.stderr.txt"
echo "exit $? ; Abzug: $(ls -la "$OUT/$NAME.bmp" 2>&1)"
cp "$EXE_DIR/debug.log" "$OUT/$NAME.debug.log" 2>/dev/null
