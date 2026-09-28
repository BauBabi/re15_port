#!/usr/bin/env bash
# Messlauf ROOM11F0 Generator-Raetsel im echten Spiel (PC-exe), Eingabe per Skript.
# Aufruf: lauf.sh <verzeichnis> <exit-bild> [framedump-spec]
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r31_generator
D="$1"; EX="$2"; FD="${3:-}"
mkdir -p "$D"; cd "$D"; rm -f panel.log debug.log fd*.ppm
S=""
for i in $(seq 1 8); do S="${S}A0.5,W0.1,"; done   # Meldungen 0/1 schreiben + blaettern
S="${S}A0.07,W1.1333,"                               # Ja bestaetigen, bis F500 warten
S="${S}U0.3,W0.2,A0.07,W4,"                          # Schalter 7 (+20) -> 20
S="${S}D0.7333,W0.2,A0.07,W4,"                       # Schalter 9 (+30) -> 50
S="${S}L1.1333,W0.2,U0.3667,W0.2,A0.07,W4,"          # Schalter 3 (-10) -> 40
S="${S}U0.7333,W0.2,A0.07,W4,"                       # Schalter 1 (+20) -> 60
S="${S}D1.4667,W0.2,A0.07,W12"                       # Schalter 5 (+20) -> 80
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
    RE15_DEBUG_JUMP="11F0@240" RE15_FIRE_AOT="1@40#11F0" \
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=320 RE15_INPUT_SCRIPT="$S" \
    RE15_PANEL_LOG=panel.log ${FD:+RE15_FRAMEDUMP="$FD"} RE15_EXIT_AT="${EX}#11F0" \
    timeout 400 "$BAUM/re15_port/build/platform/pc/re15_pc.exe" > out.txt 2>&1
echo "EXIT=$?"
