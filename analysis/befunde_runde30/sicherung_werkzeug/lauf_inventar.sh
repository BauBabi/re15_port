#!/usr/bin/env bash
# Runde 30 / Thema H (Fortsetzungs-Agent) — was zeigt das INVENTAR fuer Item 0x40?
#
#   $1 = Marke   $2 = "grid" (Raster mit Icon, Abzug F50) | "check" (CHECK-Foto, Abzug F140)
#   $3 = optional "weg2": Mess-Wurzel build/r30_sicherung/cd_weg2 (Ersatzbilder) vorschalten
#
# Schiene: die Inventar-Abnahmehaken des Ports (main.c:4325-4372) arbeiten fest auf
# Inventarplatz 0; die Mess-Variante legt die Sicherung mit RE15_R30_SLOT0=1 dorthin.
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd -W 2>/dev/null || pwd)"
MARKE="${1:?Marke fehlt}"
WIE="${2:-check}"
EXE="$ROOT/re15_port/${BAUVERZ:-build_r30_sicherung}/tests/unit/re15_pc_r30_sicherung.exe"
ZIEL="$ROOT/build/r30_sicherung/$MARKE"
mkdir -p "$ZIEL"
cd "$ZIEL" || exit 2
rm -f debug.log f_*.ppm inv.bmp
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*' MSYS2_ENV_CONV_EXCL='*'
export RE15_NOAUDIO=1 RE15_NO_INTRO=1
export RE15_TITLE_SHOT="title.bmp" RE15_TITLE_SHOT_AF=2
export RE15_R30_SLOT0="${SLOT0:-1}"
export RE15_INV_SHOT="inv.bmp"
case "$WIE" in
  grid)  export RE15_INV_GRID_SHOT=1;  export RE15_FRAMEDUMP="48-52/2:f_" ;;
  check) export RE15_INV_CHECK_SHOT=1; export RE15_FRAMEDUMP="136-144/4:f_" ;;
esac
[ "${3:-}" = "weg2" ] && export RE15_CD_ROOT="$ROOT/build/r30_sicherung/cd_weg2"
timeout -k 5 "${SEK:-40}" "$EXE"
echo "[inventar] $MARKE ($WIE ${3:-}) rc=$?  Bilder: $(ls f_*.ppm 2>/dev/null | wc -l)  inv.bmp: $(ls inv.bmp 2>/dev/null | wc -l)"
grep -n -E "r30-sicherung|\[inv\]|cd-root" debug.log | head
