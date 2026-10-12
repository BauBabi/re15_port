#!/usr/bin/env bash
# Runde 36 G2 — Messlauf an der ECHTEN exe: Generator ROOM11F0, Panel oeffnen (FIRE_AOT Slot 1 = sub16), "Ja", dann den
# Cursor per Eingabeskript an den Rahmen fahren. Vorbild: re15_port/tools/r34n_c/lauf.sh (Runde 34 Nacht, dort belegt).
# Aufruf: lauf_exe.sh <ausgabeverzeichnis> <exit-bild> <skript-rest> [framedump-spec]
#   skript-rest = RE15_INPUT_SCRIPT-Tokens NACH dem Oeffnen des Panels ("Ja" liegt bei F500; der Cursor steht dann in
#                 der Startzelle (-19554,22684) = Schalter 8).
# Schalter (getenv): RE15_DEBUG_JUMP, RE15_FIRE_AOT, RE15_INPUT_SCRIPT(+_START/_BASIS), RE15_PUSH_LOG (dachkiste_r36.c),
# RE15_PANEL_LOG (panel_zeiger_common.c), RE15_FRAMEDUMP, RE15_EXIT_AT, RE15_WINDOW_SCALE, RE15_NOAUDIO.
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r36_generator
SRC="$BAUM/re15_port/build/platform/pc/re15_pc.exe"
EXE="$BAUM/re15_port/build/platform/pc/re15_g2lauf.exe"   # eigene Kopie (local_build.sh beendet re15_pc.exe)
cp -f "$SRC" "$EXE" || exit 3
D="$1"; EX="$2"; REST="$3"; FD="${4:-}"
mkdir -p "$D"; cd "$D" || exit 2
rm -f panel.log push.txt out.txt fd_*.ppm
S=""
for i in $(seq 1 8); do S="${S}A0.5,W0.1,"; done   # Meldungen 0/1 schreiben + blaettern (sub16 @0x015A2)
S="${S}A0.07,W1.1333,"                               # "Ja" bestaetigen, bis F500 warten
S="${S}${REST}"
env SDL_ASSERT=always_ignore RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
    RE15_WINDOW_SCALE=3 \
    RE15_DEBUG_JUMP="11F0@240" RE15_FIRE_AOT="1@40#11F0" \
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=320 RE15_INPUT_SCRIPT="$S" \
    RE15_PANEL_LOG=panel.log RE15_PUSH_LOG=push.txt ${FD:+RE15_FRAMEDUMP="$FD"} RE15_EXIT_AT="${EX}#11F0" \
    timeout 600 "$EXE" > out.txt 2>&1
echo "EXIT=$?"
