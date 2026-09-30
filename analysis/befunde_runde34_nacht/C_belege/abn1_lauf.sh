#!/usr/bin/env bash
# Spur C, ABNAHME 1 (unabhaengiger Pruefer): Messlauf Generator-Raetsel ROOM11F0 an einer
# eigenen Kopie der ECHTEN exe (re15_abn1c.exe, damit kein Bau sie beendet).
# Grundgeruest wie re15_port/tools/r34n_c/lauf.sh (Panel ueber RE15_FIRE_AOT Slot 1 = sub16,
# Meldungen 0/1 blaettern, "Ja" -> Panel offen ab F500, Cursor in der Zelle von Schalter 8),
# zusaetzlich RE15_STATE_LOG (Spielerlage, Kamera, Meldung, Pausenwort je Bild).
#
# Aufruf: abn1_lauf.sh <ausgabeverzeichnis> <exit-bild> <skript-rest> [framedump-spec]
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34n_generator
EXE="$BAUM/re15_port/build/platform/pc/re15_abn1c.exe"
D="$1"; EX="$2"; REST="$3"; FD="${4:-}"
mkdir -p "$D"; cd "$D" || exit 2
rm -f panel.log state.log out.txt fd_*.ppm
S=""
for i in $(seq 1 8); do S="${S}A0.5,W0.1,"; done
S="${S}A0.07,W1.1333,"
S="${S}${REST}"
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
    RE15_WINDOW_SCALE=3 \
    RE15_DEBUG_JUMP="11F0@240" RE15_FIRE_AOT="1@40#11F0" \
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=320 RE15_INPUT_SCRIPT="$S" \
    RE15_PANEL_LOG=panel.log RE15_STATE_LOG=state.log \
    ${FD:+RE15_FRAMEDUMP="$FD"} RE15_EXIT_AT="${EX}#11F0" \
    timeout 900 "$EXE" > out.txt 2>&1
echo "EXIT=$?"
