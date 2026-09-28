#!/usr/bin/env bash
# Runde 30 / Thema H (Fortsetzungs-Agent) — LADE-WEG mit der MESS-VARIANTE.
#
# Nimmt die Karte aus build/r30_sicherung/lauf_laden/ (dort von lauf_laden.sh Schritt 1
# geschrieben: neues Spiel, Sprung ROOM1150, Save) und laedt sie in einem frischen Prozess.
#   $1 = Marke     $2 = "bestand" (Variante ohne Zusatz = Verhalten wie sicherung_1150.c)
#                       "nach"    (RE15_R30_NACHINSTALL=1: Prop im ersten Spielbild nachlegen)
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd -W 2>/dev/null || pwd)"
MARKE="${1:?Marke fehlt}"
WIE="${2:-bestand}"
EXE="$ROOT/re15_port/${BAUVERZ:-build_r30_sicherung}/tests/unit/re15_pc_r30_sicherung.exe"
ZIEL="$ROOT/build/r30_sicherung/$MARKE"
mkdir -p "$ZIEL"
cd "$ZIEL" || exit 2
rm -f debug.log f_*.ppm re15_card.mcr
cp -f "$ROOT/build/r30_sicherung/lauf_laden/re15_card.mcr" . || exit 3
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1
export RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
export RE15_FIRE_AOT="1@90#1150"
export RE15_FRAMEDUMP="90-330/10:f_"
[ "$WIE" = "nach" ] && export RE15_R30_NACHINSTALL=1
[ -n "${EXTRA_ENV:-}" ] && eval "export $EXTRA_ENV"
timeout -k 5 "${SEK:-40}" "$EXE"
echo "[laden2] $MARKE ($WIE) rc=$?  Bilder: $(ls f_*.ppm 2>/dev/null | wc -l)"
grep -n -E "CONTINUE|r30-sicherung|fire-aot|Cut_chg\(4\)|prop-render" debug.log
