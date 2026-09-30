#!/usr/bin/env bash
# Spur C (Runde 34 Nacht) — Messlauf Generator-Raetsel ROOM11F0 an der ECHTEN exe.
# Vorbild: analysis/befunde_runde31/generator_belege/lauf.sh (Runde 31, dort belegt).
#
# Aufruf: lauf.sh <ausgabeverzeichnis> <exit-bild> <skript-rest> [framedump-spec]
#   skript-rest = RE15_INPUT_SCRIPT-Tokens NACH dem Oeffnen des Panels ("Ja" liegt bei F500;
#                 danach steht der Cursor in der Startzelle = Schalter 8, rechts, Zeile 3).
#   framedump-spec = RE15_FRAMEDUMP, z.B. "517-610/3:fd_" (Serie, Readback vor Present).
#
# Schalter (getenv-Stellen gelesen): RE15_DEBUG_JUMP main.c:6982 ("<hex>@<bild>"),
# RE15_FIRE_AOT main.c:7579 ("<slot>@<bild>#<hexraum>"; Slot 1 = Panel untersuchen, sub16),
# RE15_INPUT_SCRIPT(+_START/_BASIS) input_pc.c:102-111 (30 Ticks/s; BASIS=spiel -> Spielbilder),
# RE15_FRAMEDUMP main.c:10762, RE15_EXIT_AT main.c:10793, RE15_PANEL_LOG panel_zeiger_common.c:178,
# RE15_WINDOW_SCALE render_pc.c:599 (Pflicht 3 bei Framedumps), RE15_NOAUDIO audio_pc.c:1699,
# RE15_TITLE_SHOT(+_AF) main.c:3261/3564 (Titel automatisch weiterschalten).
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34n_generator
EXE="$BAUM/re15_port/build/platform/pc/re15_r34nc.exe"   # eigene Kopie: local_build.sh beendet re15_pc.exe
D="$1"; EX="$2"; REST="$3"; FD="${4:-}"
mkdir -p "$D"; cd "$D" || exit 2
rm -f panel.log out.txt fd_*.ppm
S=""
for i in $(seq 1 8); do S="${S}A0.5,W0.1,"; done   # Meldungen 0/1 schreiben + blaettern (sub16 @0x015A2)
S="${S}A0.07,W1.1333,"                               # "Ja" bestaetigen, bis F500 warten
S="${S}${REST}"
env RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 \
    RE15_WINDOW_SCALE=3 \
    RE15_DEBUG_JUMP="11F0@240" RE15_FIRE_AOT="1@40#11F0" \
    RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=320 RE15_INPUT_SCRIPT="$S" \
    RE15_PANEL_LOG=panel.log ${FD:+RE15_FRAMEDUMP="$FD"} RE15_EXIT_AT="${EX}#11F0" \
    timeout 600 "$EXE" > out.txt 2>&1
echo "EXIT=$?"
